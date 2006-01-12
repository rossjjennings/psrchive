#include "Calibration/ReceptionModel.h"
#include "Calibration/CoherencyMeasurementSet.h"
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
  if (MEAL::LevenbergMarquardt< Jones<double> >::verbose > 2 ||
      Calibration::ReceptionModel::verbose)
    cerr << "Calibration::ReceptionModel::lmcoff" << endl;

  model.set_input_index (obs.get_input_index());

  Jones<double> delta_y = obs.get_coherency() - model.evaluate (&gradient);

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

/*! Uses the Levenberg-Marquardt algorithm of non-linear least-squares
    minimization in order to find the best fit to the observations. */
void Calibration::ReceptionModel::solve_work (bool solve_verbose)
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::solve count free parameters" << endl;

  unsigned free_params = 0;
  unsigned iparm=0;  
  for (iparm=0; iparm < get_nparam(); iparm++) {
    if (verbose) cerr << iparm  << " " << get_param(iparm) 
				<< " " << get_infit(iparm) << endl;
    if (get_infit(iparm))
      free_params ++;
  }

  vector<bool> has_source (get_num_input(), false);

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve count constraints" << endl;

  unsigned fixed_params = 0;
  for (unsigned idat=0; idat < data.size(); idat++) {
    
    // count the number of constraints provided by each CoherencyMeasurement
    fixed_params += data[idat].size() * 4;
       
    // ensure that each source_index is valid and flag the input states
    // that have at least one measurement to constrain them
    for (unsigned isource=0; isource < data[idat].size(); isource++) {
      
      unsigned source_index = data[idat][isource].get_input_index();

      if (source_index >= get_num_input())
	throw Error (InvalidRange, "Calibration::ReceptionModel::solve",
		     "isource=%d >= nsource=%d",
		     source_index, get_num_input());
      
      has_source[source_index] = true;
    }
  }
  
  if (fixed_params <= free_params) {

    for (iparm=0; iparm < get_nparam(); iparm++)
      set_Estimate (iparm, 0.0);

    throw Error (InvalidState, "Calibration::ReceptionModel::solve",
		 "ndata=%d <= nfree=%d", fixed_params, free_params);

  }

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve check constraints" << endl;

  // check that all inputs with unknown parameters have a CoherencyMeasurement
  for (unsigned ipath=0; ipath < get_num_transformation(); ipath++) {
    for (unsigned isource=0; isource < get_num_input(); isource++) {
      
      bool need_source = false;

      set_input_index (isource);
      const MEAL::Function* state = get_input ();
      
      for (unsigned iparam=0; iparam < state->get_nparam(); iparam++)
	if( state->get_infit(iparam) )
	  need_source = true;
      
      if (need_source && !has_source[isource]) {

	for (iparm=0; iparm < get_nparam(); iparm++)
	  set_Estimate (iparm, 0.0);

	throw Error (InvalidRange, "Calibration::ReceptionModel::solve",
		     "input source %d with free parameter(s) not observed",
		     isource);

      }
    }
  }

  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt< Jones<double> > fit;
  if (solve_verbose)
    fit.verbose = 1;

  // The abscissa, ordinate and ordinate error are contained in
  // Calibration::CoherencyMeasurementSet
  vector< Estimate<char> > fake (data.size());

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve compute initial fit" << endl;

  best_chisq = fit.init (data, fake, *this);

  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve chisq=" << best_chisq << endl;

  nfree = fixed_params - free_params;

// #define _DEBUG

  for (iterations = 0; iterations < maximum_iterations; iterations++) {

    float chisq = fit.iter (data, fake, *this);
    float delta_chisq = best_chisq - chisq;
    float reduced_chisq = chisq / nfree;

#ifndef _DEBUG
    if (verbose)
#endif
      cerr << "chisq=" << chisq << " delta_chisq=" << delta_chisq
           << " reduced_chisq=" << reduced_chisq
	   << " lamda=" << fit.lamda << endl;

    if (chisq < best_chisq)
      best_chisq = chisq;

    if (fit.lamda == 0.0 && delta_chisq < 1.0)  {
#ifdef _DEBUG
      cerr << "fit good" << endl;
#endif
      break;
    }

    if (delta_chisq > 0 && delta_chisq < 30) {
#ifdef _DEBUG
      cerr << "fit close" << endl; 
#endif     
      fit.lamda = 0.0;
    }

    if (exact_solution && chisq < convergence_threshold)
      break;
      
  }
 
  if (iterations >= maximum_iterations) {
    for (iparm=0; iparm < get_nparam(); iparm++)
      set_Estimate (iparm, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::solve",
		 "exceeded maximum number of iterations=%d",
		 maximum_iterations);
  }

  float reduced_chisq = best_chisq / nfree;

  if (maximum_reduced && reduced_chisq > maximum_reduced) {
    for (iparm=0; iparm < get_nparam(); iparm++)
      set_Estimate (iparm, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::solve",
		 "bad solution reduced chisq=%f", reduced_chisq);
  }

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve converged in " << iterations
	 << " iterations. chi_sq=" << best_chisq << "/(" << fixed_params
	 << "-" << free_params << "=" << nfree << ")=" 
	 << reduced_chisq << endl;
  
  try {
    fit.result (*this, covariance);
  }
  catch (Error& error) {
    for (iparm=0; iparm < get_nparam(); iparm++)
      set_Estimate (iparm, 0.0);
    throw error += "Calibration::ReceptionModel::solve";
  }

  // For data with normally-distributed errors, the variance of
  // each model parameter is given by eqn. 15.6.4 NR

  if (covariance.size() != get_nparam()) {
    for (iparm=0; iparm < get_nparam(); iparm++)
      set_Estimate (iparm, 0.0);
    throw Error (InvalidState, "Calibration::ReceptionModel::solve",
		 "MEAL::LevenbergMarquardt<Jones<double>>::result returns"
		 "\n\tcovariance matrix dimension=%d != nparam=%d",
		 covariance.size(), get_nparam());
  }

  for (iparm=0; iparm < get_nparam(); iparm++) {
    if (verbose)
      cerr << "Calibration::ReceptionModel::solve variance[" << iparm << "]="
	   << covariance[iparm][iparm] << endl;
    set_variance (iparm, covariance[iparm][iparm]);
  }

}

