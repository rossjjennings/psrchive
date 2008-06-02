/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ReceptionModel.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "MEAL/LevenbergMarquardt.h"

#include <iostream>

using namespace std;

// ///////////////////////////////////////////////////////////////////////////
//
// template specializations that enable the use of MEAL::LevenbergMarquardt
//
// ///////////////////////////////////////////////////////////////////////////

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
    cerr << "Calibration::ReceptionModel::lmcoff input index=" 
	 << obs.get_input_index() << endl;

  model.set_input_index (obs.get_input_index());

  Jones<double> result = model.evaluate (&gradient);

  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff"
      "\n  data=" << obs.get_coherency() <<
      "\n  model=" << result << endl;

  Jones<double> delta_y = obs.get_coherency() - result;

  /* Note that Calibration::CoherencyMeasurement implements the interface
     of the WeightingScheme template class used by LevenbergMacquardt */
  return MEAL::lmcoff1 (model, delta_y, obs, gradient, alpha, beta);
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
{
  if (Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff set abscissa" << endl;

  // set the independent variables for this set of measurements
  data.set_coordinates();
  // set the signal path through which these measurements were observed
  model.set_transformation_index (data.get_transformation_index());

  double chisq = 0.0;
  for (unsigned ist=0; ist<data.size(); ist++)
    chisq += lmcoff (model, data[ist], ignored, gradient, alpha, beta);

  return chisq;
}


void Calibration::ReceptionModel::old_solve ()
{
  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt< Jones<double> > fit;

  // get info from all of the MEAL classes
  // MEAL::Function::verbose = 1;

  // get info from the LevenbergMarquardt algorithm
  // fit.verbose = 3

  // get info from this method
  // fit_debug = true;

  // The abscissa, ordinate and ordinate error are contained in
  // Calibration::CoherencyMeasurementSet
  vector< Estimate<char> > fake (data.size());

  if (verbose)
    cerr << "Calibration::ReceptionModel::old_solve"
      " compute initial fit" << endl;

  best_chisq = fit.init (data, fake, *this);

  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;

  if (verbose)
    cerr << "Calibration::ReceptionModel::old_solve chisq="
	 << best_chisq << endl;

  nfree = ndat_total - nparam_infit;

  float last_lamda = 0.0;

  unsigned stick_to_steepest_decent = 0;
  unsigned patience = 5;

  for (iterations = 0; iterations < maximum_iterations; iterations++)
  {
    float chisq = fit.iter (data, fake, *this);

    if (fit_debug)
      cerr << "ITERATION: " << iterations << endl;

    if (exact_solution)
    {
      if (fit_debug)
	cerr << "chisq=" << chisq << " convergence="
	     << convergence_threshold << endl;

      if (chisq < convergence_threshold)
	break;
      else
	continue;
    }

    float delta_chisq = chisq - best_chisq;
    float reduced_chisq = chisq / nfree;

    if (verbose || fit_debug)
      cerr << "chisq=" << chisq << " delta_chisq=" << delta_chisq
           << " reduced_chisq=" << reduced_chisq
	   << " lamda=" << fit.lamda << endl;

    if (chisq < best_chisq)
      best_chisq = chisq;

    bool reiterate = false;
    for (unsigned i=0; i < convergence_condition.size(); i++)
      if ( !convergence_condition[i](this) )
	reiterate = true;

    if (reiterate)
      continue;

    if (fit.lamda == 0.0 && fabs(delta_chisq) < 1.0 && delta_chisq <= 0)
    {
      if (fit_debug)
	cerr << "fit good" << endl;
      break;
    }

    if (fit.lamda == 0.0 && delta_chisq > 0)
    {
      if (fit_debug)
	cerr << "maybe not so good" << endl;
      fit.lamda = last_lamda;

      // count when Newton's method seems to be doing very poorly
      stick_to_steepest_decent ++;
    }

    if (delta_chisq <= 0 && fabs(delta_chisq) < 10)
    {
      if (fit_debug)
	cerr << "fit close" << endl;

      if (stick_to_steepest_decent >= 5)
      {
	if (iterations >= maximum_iterations/2 &&
	    fabs(delta_chisq)/best_chisq < 1e-3)
	{
	  if (fit_debug)
	    cerr << "small change in late stages.  patience="
		 << patience << endl;

	  patience --;

	  if (!patience)
	  {
	    if (fit_debug)
	      cerr << "no more patience" << endl;
	    break;
	  }
	}

	if (fit_debug)
	  cerr << "remain patient!" << endl;
      }
      else
      {
	if (fit_debug)
	  cerr << "go for it!" << endl;
	if (fit.lamda != 0)
	  last_lamda = fit.lamda;
	fit.lamda = 0.0;
      }
    }
  }
 
  unsigned iparam = 0;

  if (iterations >= maximum_iterations)
  {
    if (fit_debug)
      cerr << "maximum iterations exceeded" << endl;
    for (iparam=0; iparam < get_nparam(); iparam++)
      set_Estimate (iparam, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::old_solve",
		 "exceeded maximum number of iterations=%d",
		 maximum_iterations);
  }

  float reduced_chisq = best_chisq / nfree;

  if (report_chisq)
    cerr << "  reduced chisq " << reduced_chisq << endl;

  if (!finite(reduced_chisq) ||
      maximum_reduced && reduced_chisq > maximum_reduced)
  {
    for (iparam=0; iparam < get_nparam(); iparam++)
      set_Estimate (iparam, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::old_solve",
		 "bad reduced chisq=%f (nfree=%d)", reduced_chisq, nfree);
  }

  if (verbose)
    cerr << "Calibration::ReceptionModel::old_solve converged in " 
	 << iterations << " iterations. chi_sq=" 
	 << best_chisq << "/(" << ndat_total << "-" << nparam_infit
	 << "=" << nfree << ")=" << reduced_chisq << endl;
  
  try {
    fit.result (*this, covariance);
  }
  catch (Error& error) {
    for (iparam=0; iparam < get_nparam(); iparam++)
      set_Estimate (iparam, 0.0);
    throw error += "Calibration::ReceptionModel::old_solve";
  }

  // For data with normally-distributed errors, the variance of
  // each model parameter is given by eqn. 15.6.4 NR

  if (covariance.size() != get_nparam())
  {
    for (iparam=0; iparam < get_nparam(); iparam++)
      set_Estimate (iparam, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::old_solve",
		 "MEAL::LevenbergMarquardt<Jones<double>>::result returns"
		 "\n\tcovariance matrix dimension=%d != nparam=%d",
		 covariance.size(), get_nparam());
  }

  for (iparam=0; iparam < get_nparam(); iparam++)
  {
    if (verbose)
      cerr << "Calibration::ReceptionModel::old_solve"
	" variance[" << iparam << "]=" << covariance[iparam][iparam] << endl;
    set_variance (iparam, covariance[iparam][iparam]);
  }

  for (unsigned i=0; i < acceptance_condition.size(); i++)
    if ( !acceptance_condition[i](this) )
      throw Error (InvalidState, "Calibration::ReceptionModel::old_solve",
		   "model not accepted by condition #%u", i);

}

