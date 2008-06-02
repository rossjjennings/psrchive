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

static void model_set (Calibration::ReceptionModel* model, const gsl_vector* x)
{
  unsigned ifit=0;

  for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
  {
    if (model->get_infit(iparam))
    {
      model->set_param( iparam, gsl_vector_get (x, ifit) );
      ifit ++;
    }
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
    model->set_transformation_index (mset.get_transformation_index());

    for (unsigned istate=0; istate<mset.size(); istate++)
    {
      model->set_input_index( mset[istate].get_input_index() );

      Jones<double> result = model->evaluate (grad_ptr);

      Jones<double> delta_y = mset[istate].get_coherency() - result;

      Jones<double> w_delta_y = mset[istate].get_weighted_conjugate (delta_y);

      /*

      Eq.(5) of van Straten (2004) or Eq.(10) of van Straten (2006)
      (summation over k is performed)

      */
      if (f)
	gsl_vector_set( f, idat, mset[istate].get_weighted_norm (delta_y) );

      // may need to normalize by
      float norm = mset[istate].get_nconstraint ();

      unsigned ifit=0;

      if (J)
      {
	for (unsigned iparam=0; iparam < model->get_nparam(); iparam++)
        {
	  if (model->get_infit(iparam))
	  {
	    /* 

	    Eq.(6) of van Straten (2004)
	    
	    */
	    gsl_matrix_set( J, idat, ifit,
			    traits.to_real (w_delta_y * gradient[iparam]) );

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

void Calibration::ReceptionModel::gsl_solve ()
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve" << endl;

  gsl_multifit_function_fdf function;
  function.f = &model_f;
  function.df = &model_df;
  function.fdf = &model_fdf;
  function.n = get_ndat_constraint ();
  function.p = get_nparam_infit ();
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

  unsigned iter = 0;
  int status = 0;

  if (verbose)
    cerr << "Calibration::ReceptionModel::gsl_solve enter loop" << endl;

  do
  {
    iter++;

    status = gsl_multifit_fdfsolver_iterate (solver);

    if (status)
      break;

    status = gsl_multifit_test_delta (solver->dx, solver->x,
				      1e-4, 1e-4);
  }
  while (status == GSL_CONTINUE && iter < 500);

  gsl_matrix *covar = gsl_matrix_alloc (function.p, function.p);
  gsl_multifit_covar (solver->J, 0.0, covar);
}

