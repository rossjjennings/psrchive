/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/ReceptionModelReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_blas.h>

#include <iostream>
#include <assert.h>

using namespace std;
using Calibration::CoherencyMeasurementSet;

static bool model_verbose = false;

static void model_set (Calibration::ReceptionModel* model, const gsl_vector* x)
{
  unsigned ifit=0;

  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    if (model->get_infit(iparam))
    {
      assert (ifit < x->size);
      model->set_param( iparam, gsl_vector_get (x, ifit) );
      ifit ++;
    }

    if (model_verbose)
      cerr << "set." << iparam << "=" << model->get_param(iparam) << endl;
  }

  assert( ifit == model->get_nparam_infit() );
}

static void model_get (const Calibration::ReceptionModel* model, gsl_vector* x)
{
  unsigned ifit=0;

  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    if (model->get_infit(iparam))
    {
      assert (ifit < x->size);
      gsl_vector_set( x, ifit, model->get_param (iparam) );
      ifit ++;
    }
  }

  assert( ifit == model->get_nparam_infit() );
}


static int model_fdf (const gsl_vector* x, void* data,
		      gsl_vector* f, gsl_matrix* J)
{
  // cerr << "model_fdf" << endl;

  Calibration::ReceptionModel* model = (Calibration::ReceptionModel*) data;

  model_verbose = false;
  model_set (model, x);

  // number of CoherencyMeasurementSets
  unsigned nset = model->get_ndata ();

  //! The traits of the gradient element
  ElementTraits< Jones<double> > traits;

  vector< Jones<double> > gradient;
  vector< Jones<double> >* grad_ptr = 0;
  if (J)
    grad_ptr = &gradient;

  unsigned idat = 0;

  for (unsigned iset=0; iset < nset; iset++)
  {
    const CoherencyMeasurementSet& mset = model->get_data (iset);

    // set the independent variables for this set of measurements
    mset.set_coordinates();

    // set the signal path through which these measurements were observed
    model->set_transformation_index( mset.get_transformation_index() );

    for (unsigned istate=0; istate<mset.size(); istate++)
    {
      model->set_input_index( mset[istate].get_input_index() );

      Jones<double> result = model->evaluate (grad_ptr);

      Jones<double> delta = mset[istate].get_coherency() - result;

      Jones<double> w_delta = mset[istate].get_weighted_conjugate (delta);

      /*

      Eq.(5) of van Straten (2004) or Eq.(10) of van Straten (2006)
      (summation over k is performed)

      */
      if (f)
      {
	assert (idat < f->size);
	gsl_vector_set( f, idat, mset[istate].get_weighted_norm (delta) );
      }

      // may need to normalize by
      // float norm = mset[istate].get_nconstraint ();

      if (J)
      {
	assert (gradient.size() == model->get_nparam());

	unsigned ifit=0;

	for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
        {
	  if (model->get_infit(iparam))
	  {
	    assert (idat < J->size1);
	    assert (ifit < J->size2);

	    /* 

	    Eq.(6) of van Straten (2004)
	    
	    */
	    gsl_matrix_set( J, idat, ifit,
			    traits.to_real (w_delta * gradient[iparam]) );

	    ifit ++;
	  }
	}

	assert( ifit == model->get_nparam_infit() );
      }

      idat++;
    }
  }

  assert( idat == model->get_ndat_constraint() );

  return GSL_SUCCESS;
}

static int model_f (const gsl_vector* x, void* data, gsl_vector* f)
{
  // cerr << "model_f" << endl;

  int ret = model_fdf (x, data, f, 0);

  // cerr << "|f(x)|=" << gsl_blas_dnrm2 (f) << endl;

  return ret;
}

static int model_df (const gsl_vector* x, void* data, gsl_matrix* J)
{
  // cerr << "model_df" << endl;

  return model_fdf (x, data, 0, J);
}

void unpack_covariance (Calibration::ReceptionModel* model,
			vector< vector<double> >& covariance,
			gsl_matrix* covar)
{
  unsigned nparam = model->get_nparam();

  covariance.resize (nparam);
  for (unsigned iparam=0; iparam < nparam; iparam++)
    covariance[iparam].resize (nparam);

  unsigned idat = 0;

  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    unsigned jdat = idat;

    for (unsigned jparam=iparam; jparam < nparam; jparam++)
    {
      double val = 0;

      if (model->get_infit(iparam) && model->get_infit(jparam))
      {
	val = gsl_matrix_get( covar, idat, jdat );
	jdat ++;
      }

      covariance[iparam][jparam] = covariance[jparam][iparam] = val;
    }

    // cerr << endl;

    if (model->get_infit(iparam))
    {
      assert( jdat == covar->size2 );
      idat ++;
    }
  }

  assert( idat == covar->size1 );

#ifdef _DEBUG
  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    for (unsigned jparam=0; jparam < nparam; jparam++)
      cerr << covariance[iparam][jparam] << " ";
    cerr << endl;
  }
#endif
}

void Calibration::ReceptionModel::gsl_solve ()
{
  gsl_multifit_function_fdf function;
  function.f = &model_f;
  function.df = &model_df;
  function.fdf = &model_fdf;
  function.n = get_ndat_constraint ();
  function.p = get_nparam_infit ();

  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve"
      " nfit=" << function.p << " ndat=" << function.n << endl;

  function.params = this;

  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve set initial guess" << endl;

  gsl_vector* initial_guess = gsl_vector_alloc (function.p);
  model_get (this, initial_guess);

  const gsl_multifit_fdfsolver_type* type = gsl_multifit_fdfsolver_lmsder;

  gsl_multifit_fdfsolver* solver
    = gsl_multifit_fdfsolver_alloc (type, function.n, function.p);

  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve set solver" << endl;

  gsl_multifit_fdfsolver_set (solver, &function, initial_guess);

  int status = 0;

  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve enter loop" << endl;

  for (iterations = 0; iterations < maximum_iterations; iterations++)
  {
    status = gsl_multifit_fdfsolver_iterate (solver);

    if (status == GSL_SUCCESS)
      break;

    status = gsl_multifit_test_delta (solver->dx, solver->x,
				      1e-4, 1e-4);

    if (status == GSL_SUCCESS)
      break;
  }

  model_verbose = true;
  // unpack the final solution
  model_set (this, solver->x);

  best_chisq = gsl_blas_dnrm2 (solver->f);

  gsl_matrix *covar = gsl_matrix_alloc (function.p, function.p);
  gsl_multifit_covar (solver->J, 0.0, covar);

  unpack_covariance (this, covariance, covar);

  gsl_multifit_fdfsolver_free (solver);
  gsl_matrix_free (covar);
  gsl_vector_free (initial_guess);
}

