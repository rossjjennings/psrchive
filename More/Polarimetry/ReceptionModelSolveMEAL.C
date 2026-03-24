/***************************************************************************
 *
 *   Copyright (C) 2004 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ReceptionModelSolveMEAL.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "MEAL/LevenbergMarquardt.h"

#if HAVE_GSL
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>
#endif

#include <iostream>

using namespace std;

std::string Calibration::SolveMEAL::get_name () const
{
  return "MEAL";
}

Calibration::SolveMEAL* Calibration::SolveMEAL::clone () const
{
  return new SolveMEAL (*this);
}

// ///////////////////////////////////////////////////////////////////////////
//
// template specializations that enable the use of MEAL::LevenbergMarquardt
//
// ///////////////////////////////////////////////////////////////////////////

//! Partial specialization for Jones elements
template< class E > struct ElementTraits< Jones<E> >
{
  //! How to cast a complex type to the Jones element type
  template< class T >
  static inline Jones<E> from_complex (const std::complex<T>& value)
  { return value; }

  /*! 2023-July-09 -- Willem van Straten
   *  Factor of 2 justified in Appendix A.2 of Rogers et al (2023) */
  static inline double to_real (const Jones<E>& element)
  { return 2.0 * trace(element).real(); }
};

// template specialization of MEAL::lmcoff
float lmcoff (// input
	      Calibration::ReceptionModel& model,
	      const Calibration::CoherencyMeasurement& obs,
	      const Estimate<char>& ignored,
	      // storage
	      vector<Jones<double> >& gradient,
	      // output
	      vector<vector<double> >& alpha,
	      vector<double>& beta)
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff input index=" << obs.get_input_index() << endl;

  /*
    Calibration::CoherencyMeasurement has an index, such as the pulse
    phase bin number or harmonic number (the x-axis-like abscissa).
  */

  model.set_input_index (obs.get_input_index());

  Jones<double> result = model.evaluate (&gradient);

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff"
      "\n  data=" << obs.get_coherency() <<
      "\n  model=" << result << endl;

  /*
    Calibration::CoherencyMeasurement also contains the measured value
    (the y-axis-like ordinate).
  */
  Jones<double> delta_y = obs.get_coherency() - result;

#if 0
  if (generalized template matching)
  {
    /*
      compute the contribution to chisq, alpha, and beta corresponding
      to a single index in the vector product that includes the GTM
      covariance matrix.
      
      the contributions to alpha and beta will include the partial
      derivatives of the model (gradient) as well as the partial
      derivatives of the covariance matrix (all with respect to the
      free parameters in the fit).
    */

    return chisq;
  }
#endif

  /* 
     Calibration::CoherencyMeasurement implements the interface
     of the WeightingScheme template class used by LevenbergMacquardt.
     The weight depends on the error (sigma-like).
  */
  try
  {
    return MEAL::lmcoff1 (model, delta_y, obs, gradient, alpha, beta);
  }
  catch (Error& error)
  {
    error << "\n\t" "data=" << obs.get_coherency() 
          << "\n\t" "model=" << result;
    throw error += "Calibration::ReceptionModel::lmcoff";
  }
}

// template specialization of MEAL::lmcoff
float lmcoff (// input
	      Calibration::ReceptionModel& model,
	      const Calibration::CoherencyMeasurementSet& data,
	      const Estimate<char>& ignored,
	      // storage
	      vector<Jones<double> >& gradient,
	      // output
	      vector<vector<double> >& alpha,
	      vector<double>& beta)
try
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff set abscissa" << endl;

  // set the common independent variables for this set of measurements
  data.set_coordinates();
  // set the signal path through which these measurements were observed
  model.set_transformation_index (data.get_transformation_index());

  double chisq = 0.0;
  for (unsigned ist=0; ist<data.size(); ist++)
  {
    // set the independent variables for this measurement
    data[ist].set_coordinates();
    chisq += lmcoff (model, data[ist], ignored, gradient, alpha, beta);
  }
  
  return chisq;
}
catch (Error& error)
{
  throw error += "Calibration::ReceptionModel::lmcoff 2";
}

void Calibration::SolveMEAL::fit ()
{
  if (!equation)
    throw Error (InvalidState, "Calibration::SolveMEAL::fit", "no equation");

  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt< Jones<double> > fit;

  // get info from all of the MEAL classes
  // MEAL::Function::verbose = 1;

  // get info from the LevenbergMarquardt algorithm
  fit.verbose = verbose;

  // get info from this method
  // debug = true;

  // The abscissa, ordinate and ordinate error are contained in
  // Calibration::CoherencyMeasurementSet
  vector< Estimate<char> > fake (get_data().size());

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::SolveMEAL::fit compute initial fit" << endl;

  try
  {
    best_chisq = fit.init (get_data(), fake, *equation);
  }
  catch (Error& error)
  {
    throw error += "Calibration::SolveMEAL::fit (init)";
  }

  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::SolveMEAL::fit chisq=" << best_chisq << endl;

  float last_lamda = 0.0;

  unsigned stick_to_steepest_decent = 0;
  unsigned patience = 5;

  for (iterations = 0; iterations < maximum_iterations; iterations++) try
  {
    float chisq = fit.iter (get_data(), fake, *equation);

    if (debug)
      cerr << "ITERATION: " << iterations << endl;

    if (convergence_chisq)
    {
      if (debug)
      	cerr << "chisq=" << chisq << " convergence=" << convergence_chisq << endl;

      if (chisq < convergence_chisq)
        break;
      else
        continue;
    }

    float delta_chisq = chisq - best_chisq;
    float reduced_chisq = chisq / nfree;

    if (Calibration::ReceptionModel::verbose || debug)
      cerr << "chisq=" << chisq << " delta_chisq=" << delta_chisq
           << " reduced_chisq=" << reduced_chisq << " lamda=" << fit.lamda << endl;

    if (chisq < best_chisq)
      best_chisq = chisq;

    bool reiterate = false;
    for (unsigned i=0; i < convergence_condition.size(); i++)
      if ( !convergence_condition[i](equation) )
        reiterate = true;

    if (reiterate)
      continue;

    if (fit.lamda == 0.0 && fabs(delta_chisq) < 1.0 && delta_chisq <= 0)
    {
      if (debug)
        cerr << "fit good" << endl;
      break;
    }

    if (fit.lamda == 0.0 && delta_chisq > 0)
    {
      if (debug)
        cerr << "maybe not so good" << endl;
      fit.lamda = last_lamda;

      // count when Newton's method seems to be doing very poorly
      stick_to_steepest_decent ++;
    }

    if (delta_chisq <= 0 && fabs(delta_chisq) < 10)
    {
      if (debug)
        cerr << "fit close" << endl;

      if (stick_to_steepest_decent >= 5)
      {
        if (iterations >= maximum_iterations/2 && fabs(delta_chisq)/best_chisq < 1e-3)
        {
          if (debug)
            cerr << "small change in late stages.  patience=" << patience << endl;

          patience --;

          if (!patience)
          {
            if (debug)
              cerr << "no more patience" << endl;
            break;
          }
        }

        if (debug)
          cerr << "remain patient!" << endl;
      }
      else
      {
        if (debug)
          cerr << "go for it!" << endl;
        if (fit.lamda != 0)
          last_lamda = fit.lamda;
        fit.lamda = 0.0;
      }
    }
  }
  catch (Error& error)
  {
    /* Each iterative step includes inversion of the Hessian matrix.
       If this fails, then it is likely singular (i.e. there is an
       ill-constrained free parameter). */

    singular = true;
    error << "\n\t" "iteration=" << iterations;
    throw error += "Calibration::SolveMEAL::fit";
  }

  if (iterations == maximum_iterations)
    return;

  std::vector<std::vector<double> > curvature;

  try
  {
    fit.result (*equation, covariance, curvature);
  }
  catch (Error& error)
  {
    error << "\n\t" "result";
    throw error += "Calibration::SolveMEAL::fit";
  }

  if (fit.get_nparam_infit() != nparam_infit)
    throw Error (InvalidState, "Calibration::SolveMEAL::fit",
                "nparam_infit=%u != that returned by fit algorithm =%u",
                nparam_infit, fit.get_nparam_infit());

  double log_det_curvature = fit.get_log_det_curvature();
  /* the curvature matrix in Levenberg-Marquardt is one half of the Hessian */
  log_det_Hessian = log_det_curvature + nfree * log(2.0);

#if HAVE_GSL

  assert(curvature.size() >= nparam_infit);

  DEBUG("Calibration::SolveMEAL::fit allocate arrays");
  gsl_matrix *m = gsl_matrix_alloc(nparam_infit, nparam_infit);
  gsl_vector *eval = gsl_vector_alloc(nparam_infit);
  gsl_matrix *evec = gsl_matrix_alloc(nparam_infit, nparam_infit);
  gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(nparam_infit);

  DEBUG("Calibration::SolveMEAL::fit copy");
  unsigned nparam = curvature.size();
  unsigned in_iparam = 0;
  for (unsigned iparam=0; iparam<nparam; iparam++)
  {
    if (!equation->get_infit(iparam))
      continue;

    unsigned in_jparam = 0;
    for (unsigned jparam=0; jparam<=iparam; jparam++)
    {
      if (!equation->get_infit(jparam))
        continue;

      gsl_matrix_set(m, in_iparam, in_jparam, curvature[iparam][jparam]);
      gsl_matrix_set(m, in_jparam, in_iparam, curvature[iparam][jparam]);

      in_jparam ++;
    }

    in_iparam ++;
  }

  DEBUG("Calibration::SolveMEAL::fit gsl_eigen_symmv");
  gsl_eigen_symmv(m, eval, evec, w);
  gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);

  double lambda_max = gsl_vector_get(eval, 0);
  double lambda_min = lambda_max;

  double plateau_threshold = 1.01;
  double nullspace_threshold = 1e-3;
  unsigned ndim_nullspace = 0;

  const int falling = 0;
  const int plateau = 1;
  const int nullspace = 2;

  int state = falling;

  if (verbose)
    cerr << "Calibration::SolveMEAL::fit testing " << nparam_infit << " eigenvalues" << endl;

  double log_det = 0.0;

  for (unsigned i=0; i<nparam_infit; i++)
  {
    double lambda = gsl_vector_get(eval, i);

    if (state == falling && lambda > 0.0 && lambda_min / lambda < plateau_threshold)
    {
      if (verbose)
        cerr << "entering plateau at i=" << i << " lambda=" << lambda << " lambda_min=" << lambda_min << endl;
      state = plateau;
    }
    else if (state == plateau && lambda / lambda_min < nullspace_threshold)
    {
      if (verbose)
        cerr << "entering nullspace at i=" << i << " fall=" << lambda/lambda_min << endl;
      state = nullspace;
      ndim_nullspace = nparam_infit - i;
    }

    if (state != nullspace)
    {
      lambda_min = lambda;
      log_det += log(lambda);
    }
    else if (verbose)
    {
      cerr << "lambda_" << i << "=" << lambda << endl;

      unsigned in_iparam = 0;
      for (unsigned iparam=0; iparam<equation->get_nparam(); iparam++)
      {
        if (!equation->get_infit(iparam))
          continue;

        double val = gsl_matrix_get(evec, i, in_iparam);
        if (fabs(val) > 1e-5)
          cerr << in_iparam << " " << equation->get_param_name(iparam)
              << " " << val << endl;

        in_iparam ++;
      }
    }
  }

  log_cond_Hessian = log(lambda_max) - log(lambda_min);

  if (verbose)
  {
    cerr << "lambda_max=" << lambda_max << endl;
    cerr << "lambda_min=" << lambda_min << endl;
    cerr << "log(condition) of Hessian =" << log_cond_Hessian
         << " ndim_nullspace=" << ndim_nullspace << " log(det)=" << log_det 
         << " or " << log_det_curvature << " delta=" << log_det_curvature - log_det << endl;
  }

  gsl_matrix_free(m);
  gsl_matrix_free(evec);
  gsl_vector_free(eval);
  gsl_eigen_symmv_free(w);

#endif

  if (covariance.size() != equation->get_nparam())
    throw Error (InvalidState, "Calibration::SolveMEAL::fit",
		 "MEAL::LevenbergMarquardt<Jones<double>>::result returns"
		 "\n\tcovariance matrix dimension=%d != nparam=%d",
		 covariance.size(), equation->get_nparam());
}
