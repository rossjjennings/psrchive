/***************************************************************************
 *
 *   Copyright (C) 2004-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
 
 Simulates the observation of some randomly polarized states using
 an instrument with a random Jones matrix over a range of parallactic angles,
 then fits a model to the simulated observations.  On each iteration, the 
 model of the Jones matrix toggles between Britton (2000) and Hamaker (2000).

 By default, this application runs a unit test over 100 random trials.

 Lately, I've also be using this application to explore how different models
 have different levels of parameter collinearity, as characterized by the 
 logarithm of the condition number of the Hessian matrix, log_cond.  
 e.g. to explore how log_cond varies with difficulty:

 #!/usr/bin/bash
 for difficulty in 0.2 0.3 0.4; do
  ./test_ReceptionModel -d $difficulty -b0 -o32 -s8 -i 1000 |& grep "mean log(cond(Hessian)"
 done

 The above loop takes a few minutes to complete, yielding

 mean log(cond(Hessian) Hamaker=11.4376 Britton=11.4773
 mean log(cond(Hessian) Hamaker=13.9012 Britton=14.5261
 mean log(cond(Hessian) Hamaker=15.932 Britton=16.8871

 To explore greater levels of difficulty, this code will need to be updated
 to start with a better first guess for the model parameters.
*/

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/Parallactic.h"
#include "Pulsar/CelestialProjection.h"
#include "Pulsar/CovarianceReport.h"
#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/MeanCoherency.h"
#include "MEAL/PhysicalCoherency.h"

#include "Pulsar/SingleAxis.h"
#include "Pulsar/Britton2000.h"
#include "MEAL/Polar.h"
#include "MEAL/Gain.h"
#include "MEAL/Axis.h"

#include "Pulsar/SingleAxisPolynomial.h"
#include "MEAL/Complex2Constant.h"

#include "Horizon.h"
#include "Fixed.h"
#include "Pauli.h"
#include "BoxMuller.h"
#include "strutil.h"

#include <iostream>
#include <algorithm>
#include <unistd.h>

using namespace std;

// the maximum value of sin, cos, sinh, and cosh 
// used when generating random receiver and polynomials
float difficulty = 0.2;

// the maximum degree of polarization of the source states
float max_poln = 0.8;

// tolerance to error
float error_tolerance = 2.0;

// minimum hour angle
float ha_min = -5;

// maximum hour angle
float ha_max = 5;

// number of backend polynomial coefficients
unsigned ncoef = 8;

// number of random source and receiver combinations to test
unsigned nloop = 100;

// number of input polarimetric states
unsigned nstates = 4;

// number of source observations
unsigned nobs = 12;

// number of calibrator observations
unsigned ncal = 1;

// noise added to data
float noise = 0.0;

// two models: Hamaker or Britton
bool hamaker = true;

// test the fixed dipole projection
bool test_projection = false;

// by default, the boost along Stokes V is constrained by the CAL
bool degenerate_V_boost = false;

// by default, the rotation along Stokes V is constrained by the CAL
bool degenerate_V_rotation = false;

// print model parameter covariance reports
bool print_covariance_reports = false;

// plot things
bool verbose = false;
bool vverbose= false;

// test the Geometric Akaike Information Criterion
// 1 - fit variations when there are none
// 2 - fit variations when there are some

int test_GIC = 0;

void usage ()
{
  cerr << 
    "test_ReceptionModel\n"
    "  simulates a polarimetric experiment and its calibration\n"
    "\n"
    "  -b N number of coefficients in backend polynomials (default="
       << ncoef << ")\n"
    "  -c N number of calibrator observations (default="
       << ncal << ")\n"
    "  -d x difficulty (default="
       << difficulty << ")\n"
    "  -D [b|r] constant boost/rotation along Stokes V \n"
    "  -f x maximum fractional polarization of sources (default="
       << max_poln << ")\n"
    "  -G n test the Geometric Akaike Information Criterion\n"
    "  -i N number of iterations (default="
       << nloop << ")\n"
    "  -l x hour angle range (default="
       << ha_max << ")\n"
    "  -n x standard deviation of noise (default="
       << noise << ")\n"
    "  -o N number of observations (default="
       << nobs << ")\n"
    "  -p   use the fixed dipole projection (default is parallactic)\n"
    "  -s N number of source states (default="
       << nstates << ")\n"
    "  -t x error tolerance (default="
       << error_tolerance << ")\n\n"
    "  -h   help\n"
    "  -v   verbose\n"
    "  -V   very verbose\n"
       << endl;
}



void random_receiver (Jones<double>& receiver)
{
  unsigned i = 0;
  double modp = 0;

  Quaternion<double, Hermitian> boost;
  random_vector (boost, difficulty);

  if (degenerate_V_boost)
    boost[3] = 0;

  modp=0;
  for (i=1; i<4; i++)
    modp += boost[i]*boost[i];

  boost.s0 = cosh ( asinh ( sqrt (modp) ) );

  Quaternion<double, Unitary> rotation;
  random_vector (rotation, difficulty);

  if (degenerate_V_rotation)
    rotation[3] = 0;

  modp=0;
  for (i=1; i<4; i++)
    modp += rotation[i]*rotation[i];

  rotation.s0 = cos ( asin ( sqrt (modp) ) );

  receiver = convert(boost) * rotation;

  if ( fabs(det(receiver).real() - 1.0) > 1e-8 )
    throw Error (InvalidState, "random_receiver", "det(j)!=1");
}

int compare (Jones<double> model_receiver, Jones<double> receiver, float max)
{
  if (!hamaker)
    // the determinant of the phenomenological model is not necessarily 1
    model_receiver /= sqrt(det(model_receiver));

  float diff = sqrt(norm (model_receiver - receiver));
  
  if (diff >= max) {
    // try -ve
    model_receiver *= -1;
    float new_diff = sqrt(norm (model_receiver - receiver));
    if (new_diff < diff)
      diff = new_diff;
    else
      model_receiver *= -1;
  }
  
  if (diff < max)
    return 0;

  Jones<double> factor = inv(receiver) * model_receiver;
    
  Quaternion<double, Hermitian> H;
  Quaternion<double, Unitary> U;
  complex<double> d;
  
  polar (d, H, U, factor);
  
  cerr <<
    "\nmodel receiver, R' = R * dHU, where R is the input receiver and"
    "\nd=" << d <<
    "\nH=" << H << 
    "\nU=" << U << endl;
 
  factor = model_receiver * inv(receiver);
  polar (d, H, U, factor);

  cerr <<
    "\nmodel receiver, R' = dHU * R, where R is the input receiver and"
    "\nd=" << d <<
    "\nH=" << H <<
    "\nU=" << U << endl;
 
  if (verbose)
    cerr << "\nmodel receiver=" << model_receiver << " != "
	 << "\ninput receiver=" << receiver 
	 << "\n diff receiver=" << factor
	 << " norm=" << diff << " > max_norm=" << max <<  endl;
  
  return -1;
}

// ///////////////////////////////////////////////////////////////////////
//
// generate evenly-spaced observations of states, through signal_path
// as a function of axis, starting at axis_min and ending at axis_max
//
void observe (vector<Calibration::CoherencyMeasurementSet>& observations,
              vector< Stokes<double> >& states, float variance,
              MEAL::Complex2& signal_path,
              MEAL::Axis<double>& axis, 
              double axis_min, double axis_max,
              unsigned path_index,
              unsigned start_index)
{
  BoxMuller gasdev;

  float obs_step = 1.0;

  if (observations.size() > 1)
    obs_step = float( observations.size() - 1 );

  // the time between each calibrator observation
  float axis_step = (axis_max - axis_min) / obs_step;

  for (unsigned iobs = 0; iobs < observations.size(); iobs++)
  {
    // form the rotation matrix about the V-axis
    float abscissa = axis_min + float(iobs) * axis_step;

    observations[iobs].set_transformation_index( path_index );
    observations[iobs].add_coordinate( axis.new_Value(abscissa) );
    observations[iobs].set_coordinates( );

    Jones<double> xform = signal_path.evaluate ();

    //if (vverbose)
      //cerr << "iobs=" << iobs << " xform=" << xform << endl;

    for (unsigned istate = 0; istate < states.size(); istate++)
    {
      Stokes<double> value = transform( states[istate], xform );
      Stokes< Estimate<double> > stokes;

      for (unsigned ipol=0; ipol<4; ipol++)
      {
        if (noise > 0.0)
          value[ipol] += noise * gasdev();

        stokes[ipol].val = value[ipol];
        stokes[ipol].var = variance;
      }

      // form a measurement
      Calibration::CoherencyMeasurement state (istate + start_index);
      state.set_stokes (stokes);

      // add the measurement to this coordinate
      observations[iobs].push_back (state);
    }
  }
}


float min_fracpoln_fail = 1.0;
float max_fracpoln_fail = 0.0;

MEAL::Axis<double> hour_angle;

Calibration::SingleAxisPolynomial* add_single_axis_polynomial (MEAL::ProductRule<MEAL::Complex2>& signal_path)
{
  // add a time-varying transformation to the signal path  
  auto backend = new Calibration::SingleAxisPolynomial (ncoef);
  signal_path.add_model (backend);

  // must be fixed at t=0 (or else covariant with system)
  backend->set_infit (0, false);

  // make the backend vary as a function of hour angle
  backend->set_argument (0, &hour_angle);

  return backend;
}

float AIC = 0.0;
float GIC = 0.0;
float SIC = 0.0;

void calculate_criteria(const Calibration::ReceptionModel::Solver* solver, unsigned ndim)
{
  cerr << "chisq=" << solver->get_chisq() << " nfree=" << solver->get_nfree() << " n_infit=" << solver->get_nparam_infit() << " ndat=" << solver->get_ndat_constraint() << endl;
  cerr << "log_det=" << solver->get_log_det_curvature() << " log_cond=" << solver->get_log_cond_curvature() << endl;

  AIC = solver->get_chisq() + 2.0 * solver->get_nparam_infit();

  float N = solver->get_ndat_constraint() * 0.25;
  GIC = AIC + 2.0 * ndim * N;

  SIC = solver->get_chisq() + solver->get_log_det_curvature();

  cerr << "AIC=" << AIC << " GIC=" << GIC << " SIC=" << SIC << endl;
}

float min_AIC, max_AIC = 0.0;
float min_GIC, max_GIC = 0.0;
float min_SIC, max_SIC = 0.0;

void update_min_max (float& min, float& max, float val)
{
  if (min == 0.0 || val < min)
    min = val;
  if (max == 0.0 || val > max)
    max = val;
}

double mean_AIC = 0.0;
double mean_GIC = 0.0;
double mean_SIC = 0.0;

double log_cond_Hamaker = 0.0;
unsigned count_Hamaker = 0;
double log_cond_Britton = 0.0;
unsigned count_Britton = 0;

template<typename T>
int runtest (T& projection, unsigned iteration)
{
  // connect projection.set_hour_angle to hour_angle axis
  hour_angle.signal.connect (&projection, &T::set_hour_angle);

  // the randomly-generated receiver
  Jones<double> receiver;
  random_receiver (receiver);

  // a fake variance for now (add noise later)
  float variance = 1e-4;

  if (noise > 0.0)
    variance = noise * noise;

  // the transformation through which the polarized signals are propagated
  MEAL::ProductRule<MEAL::Complex2> signal_path;

  if (ncoef && test_GIC == 2)
  {
    cerr << "Adding SingleAxisPolynomial with ncoef=" << ncoef << " to true signal path" << endl;
    auto backend = add_single_axis_polynomial (signal_path);

    backend->random(difficulty / 1000.0, ha_max);
  }

  // a constant transformation equal to the receiver
  MEAL::Complex2Constant instrument (receiver);

  signal_path.add_model (&instrument);

  // ///////////////////////////////////////////////////////////////////////
  //
  // simulate the calibrator observations
  //

  vector<Calibration::CoherencyMeasurementSet> calibrator_obs (ncal);

  cerr << "Simulating " << ncal << " calibrator observation(s)" << endl;

  // the input calibrator state
  Stokes<double> calibrator (1,0,.95,0);

  vector< Stokes<double> > source (1, calibrator);

  observe (calibrator_obs, source, variance, signal_path, hour_angle, ha_min, ha_max, 0, 0);


  // ///////////////////////////////////////////////////////////////////////
  //
  // add the projection to the signal path
  //

  // the sources are observed through an additional projection angle rotation
  signal_path.add_model ( &projection );


  // ///////////////////////////////////////////////////////////////////////
  //
  // generate random source states
  //

  vector<Calibration::CoherencyMeasurementSet> source_obs (nobs);

  cerr << "Simulating " << nobs << " source observation(s)" << endl;

  source.resize (nstates);

  float intensity = 10;
  for (unsigned istate = 0; istate < nstates; istate++)
  {
    random_value (source[istate], intensity, max_poln);
    if (verbose)
      cerr << "random source[" << istate << "]=" << source[istate] << endl;
  }

  unsigned start_index = 0;
  if (ncal)
    start_index = 1;

  observe (source_obs, source, variance, signal_path, hour_angle, ha_min, ha_max, start_index, start_index);

  // ///////////////////////////////////////////////////////////////////////
  //
  // deparallactify the observations and use them as the first guess
  //

  // the model for each of the source states
  vector< MEAL::Coherency* > source_model (nstates);

  // the best guess for each of the source states
  vector< Calibration::MeanCoherency > source_guess (nstates);

  for (unsigned iobs = 0; iobs < nobs; iobs++)
  {
    source_obs[iobs].set_coordinates();

    Jones<Estimate<double> > para = projection.evaluate ();

    if (verbose)
      cerr << "iobs=" << iobs << " projection=" << para << endl;

    for (unsigned istate = 0; istate < nstates; istate++)
    {
      // get the observation
      Stokes< Estimate<double> > state = source_obs[iobs][istate].get_stokes();

      if (vverbose)
        cerr << "state["<<istate<<"]=" << state << endl;

      // deparallactify
      state = transform( state, herm (para) );

      // add to the observed average
      source_guess[istate].integrate (state);
    }
  }

  // the model of the receiver and source state, and measurements
  Calibration::ReceptionModel model;

  if (ncal)
  {
    // ///////////////////////////////////////////////////////////////////////
    //
    // add the calibrator constraint to the model
    //

    // if (vverbose)
      cerr << "construct MEAL::PhysicalCoherency calibrator instance" << endl;
    MEAL::Coherency* state = new MEAL::PhysicalCoherency;

    state->set_stokes (calibrator);

    // disable fit flags
    for (unsigned istokes=0; istokes<4; istokes++)
      state->set_infit (istokes, false);
    
    if (vverbose)
      cerr << "add MEAL::Coherency to ReceptionModel" << endl;
    
    model.add_input (state);
  }

  // ///////////////////////////////////////////////////////////////////////
  //
  // add the source constraints to the model
  //

  for (unsigned istate = 0; istate < nstates; istate++)
  {
    source_model[istate] = new MEAL::PhysicalCoherency;

    // update the best first guess
    source_guess[istate].update (source_model[istate]);

    if (vverbose)
      cerr << "source[" << istate << "] guess="
	   << source_model[istate]->get_stokes() << endl;

    // add to the model
    model.add_input (source_model[istate]);
  }

  // ///////////////////////////////////////////////////////////////////////
  //
  // construct the instrumental response signal path
  //

  MEAL::ProductRule<MEAL::Complex2>* path;
  path = new MEAL::ProductRule<MEAL::Complex2>;

  if (ncoef && !test_GIC)
  {
    cerr << "Adding SingleAxisPolynomial with ncoef=" << ncoef << " to model signal path" << endl;
    add_single_axis_polynomial (*path);
  }

  // ///////////////////////////////////////////////////////////////////////
  //
  // add the constant transformation
  //
  MEAL::Complex2* system;

  if (hamaker)
  {
    cerr << "Using Algebraic Decomposition (Hamaker)" << endl;
    system = new MEAL::Polar;

    if (degenerate_V_boost)
    {
      cerr << "Disabling fit for " << system->get_param_name(3) << endl;
      system->set_infit (3, false); // V boost is b_3
    }
    if (degenerate_V_rotation)
    {
      cerr << "Disabling fit for " << system->get_param_name(6) << endl;
      system->set_infit (6, false); // V rotation is r_3
    }
  }
  else
  {
    cerr << "Using Phenomenological Decomposition (Britton)" << endl;
    system = new Calibration::Britton2000;

    if (degenerate_V_boost)
    {
      // disabling degenerate parameters
      cerr << "Disabling fit for " << system->get_param_name(4) << endl;
      system->set_infit (4, false); // V boost is b_2
    }
    if (degenerate_V_rotation)
    {
      cerr << "Disabling fit for " << system->get_param_name(6) << endl;
      system->set_infit (6, false); // V rotation is r_2
    }
  }

  if (!ncal)
  {
    cerr << "Disabling fit for " << system->get_param_name(0) << endl;
    system->set_infit (0, false); // gain
  }

  path->add_model (system);

  unsigned cal_path = 0;
  unsigned src_path = 0;

  if (ncal)
  {
    // ///////////////////////////////////////////////////////////////////////
    //
    // add the calibrator signal path to the model
    //

    cal_path = model.get_transformation_index();

    model.add_transformation (path);

    path = new MEAL::ProductRule<MEAL::Complex2> (*path);
  }

  // ///////////////////////////////////////////////////////////////////////
  //
  // add the source signal path to the model
  //

  path->add_model( &projection );

  src_path = model.get_transformation_index();

  model.add_transformation (path);

  cerr << "ReceptionModel::nparam=" << model.get_nparam() << endl;

  // ///////////////////////////////////////////////////////////////////////
  //
  // add the projection angle rotation
  //
  
  model.set_transformation_index (cal_path);
  for (unsigned ical=0; ical < calibrator_obs.size(); ical++)
    model.add_data (calibrator_obs[ical]);

  model.set_transformation_index (src_path);
  for (unsigned isrc=0; isrc < source_obs.size(); isrc++)
    model.add_data (source_obs[isrc]);


  // ///////////////////////////////////////////////////////////////////////
  //
  // solve the model!
  //

  if (noise == 0.0)
  {
    // original fake variance kludge
    model.get_solver()->set_convergence_chisq (variance*variance);
  }

  try
  {
    if (verbose || test_GIC)
      cerr << "runtest call Calibration::ReceptionModel::solve" << endl;
    model.solve ();
  }
  catch (Error& error)
  {
    cerr << error << endl;
    return -1;
  }

  double log_cond = model.get_solver()->get_log_cond_curvature(); 
  cerr << "log(condition(Hessian)) " << log_cond << endl;

  if (hamaker)
  {
    log_cond_Hamaker += log_cond;
    count_Hamaker ++;
  }
  else
  {
    log_cond_Britton += log_cond;
    count_Britton ++;
  }

  unsigned nparam = system->get_nparam();
  for (unsigned iparam=0; iparam < nparam; iparam++)
    cerr << iparam << " " << system->get_param_name(iparam) << " " << system->get_Estimate(iparam) << endl;

  if (print_covariance_reports)
  {
    string filename = "covariance_" + stringprintf("%03d", iteration) + ".txt";
    Calibration::CovarianceReport report (filename);
    report.set_model(&model);
    report.Calibration::ReceptionModel::Report::report();
  }

  float AIC_0 = 0.0;
  float GIC_0 = 0.0;
  float SIC_0 = 0.0;

  if (ncoef && test_GIC)
  {
    calculate_criteria(model.get_solver(), 0);

    AIC_0 = AIC;
    GIC_0 = GIC;
    SIC_0 = SIC;
    
    cerr << "Adding SingleAxisPolynomial with ncoef=" << ncoef << " to model signal path after first solution" << endl;
    add_single_axis_polynomial (*path);

    cerr << "Calling Calibration::ReceptionModel::solve again" << endl;

    try
    {
      cerr << "runtest call Calibration::ReceptionModel::solve" << endl;
      model.solve ();
    }
    catch (Error& error)
    {
      cerr << error << endl;
      return -1;
    }

    calculate_criteria(model.get_solver(), 1);

    float delta_AIC = AIC - AIC_0;
    update_min_max (min_AIC, max_AIC, delta_AIC);
    mean_AIC += delta_AIC;

    float delta_GIC = GIC - GIC_0;
    update_min_max (min_GIC, max_GIC, delta_GIC);
    mean_GIC += delta_GIC;
    
    float delta_SIC = SIC - SIC_0;
    update_min_max (min_SIC, max_SIC, delta_SIC);
    mean_SIC += delta_SIC;

    return 0;
  }

  float limit = error_tolerance * error_tolerance * variance;
 
  if (verbose)
    cerr << "runtest call compare" << endl;

  float max_norm = limit * norm (receiver);
  if (compare (system->evaluate(), receiver, max_norm) < 0)
    return -1;

  if (vverbose)
  {
    for (unsigned istate=0; istate<nstates; istate++)
      cerr << "source["<<istate<<"] = " << source[istate]
	   << "\ninitial guess[" << istate << "] = " 
	   << source_guess[istate].get_mean()
	   << "\nguess source[" << istate << "] = " 
	   << source_model[istate]->evaluate() << endl;
  }

    
  // complex<double> Gain = sqrt (det(model_receiver));
  // model_receiver /= Gain;
  
  bool error = false;

  if (verbose)
    cerr << "runtest verify model states" << endl;

  for (unsigned istate=0; istate<nstates; istate++)
  {
    Stokes<double> state = coherency( source_model[istate]->evaluate() );
    
    if (vverbose)
      cerr << "runtest check state=" << istate << endl
	   << "   model=" << state << endl
	   << "   input=" << source[istate] << endl;


    float max_norm = limit * norm (source[istate]);
    float diff = sqrt(norm (state - source[istate]));
    
    if (diff > max_norm)
    {
      cerr << "\n\nmodel source[" << istate << "]=" << state << " != "
	   << "\ninput source[" << istate << "]=" << source[istate] 
	   << " norm=" << diff << " > max_norm=" << max_norm <<  endl;

      if (verbose)
      {
        model.set_input_index (istate);
        
        for (unsigned iobs=0; iobs<nobs; iobs++)
        {
          Calibration::CoherencyMeasurementSet& meas = source_obs[iobs];
          
          for (unsigned imeas=0; imeas<meas.size(); imeas++)
          {
            if (meas[imeas].get_input_index() == istate)
            {
              meas.set_coordinates ();
              
              vector< Jones<double> > grad;
              Stokes<double> observed_source;
              
              observed_source = coherency( model.evaluate (&grad) );
              
              cerr << "obsout["<<iobs<<"]=" << observed_source
            << "\n obsin["<<iobs<<"]=" 
            << meas[imeas].get_stokes() << endl;
            }
          } // for each measurement
        } // for each observation
      } // if verbose
          
      float fracpoln = source[istate].abs_vect () / source[istate][0];
      
      if (fracpoln < min_fracpoln_fail)
        min_fracpoln_fail = fracpoln;
      
      if (fracpoln > max_fracpoln_fail)
        max_fracpoln_fail = fracpoln;
      
      error = true;
    }
  }
  
  if (error)
    return -1;

  if (verbose)
    cerr << "Success: model states equal input states" << endl;

  return 0;
}


int main (int argc, char** argv)
{
  int c = 0;
  const char* args = "b:c:d:D:f:G:hi:l:n:o:ps:t:vV";
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'b':
      ncoef = atoi (optarg);
      break;

    case 'c':
      ncal = atoi (optarg);
      break;

    case 'd':
      difficulty = atof (optarg);
      if (difficulty * sqrt(3.0) > 1.0 || difficulty < 0.0)
        cerr << "difficulty must be >0 and <" << sqrt(3.0) << endl;
      break;

    case 'D':
      if (optarg[0] == 'b')
        degenerate_V_boost = true;
      else if (optarg[0] == 'r')
        degenerate_V_rotation = true;
      break;

    case 'f':
      max_poln = atof (optarg);
      if (max_poln > 1.0 || max_poln < 0.0)
        cerr << "maximum fractional polarization must be >0 and <1" << endl;
      break;

    case 'G':
      test_GIC = atoi (optarg);
      break;

    case 'i':
      nloop = atoi (optarg);
      break;

    case 'l':
      ha_max = atof (optarg);
      ha_min = -ha_max;
      break;

    case 'n':
      noise = atof (optarg);
      break;

    case 'o':
      nobs = atoi (optarg);
      break;

    case 'p':
      test_projection = true;
      break;

    case 's':
      nstates = atoi (optarg);
      break;

    case 't':
      error_tolerance = atof (optarg);
      break;

    case 'h':
      usage ();
      return 0;

    case 'V':
      //MEAL::Function::very_verbose = true;
      MEAL::Function::verbose = true;
      MEAL::Argument::verbose = true;
      vverbose = true;
    case 'v':
      verbose = true;
      break;

    }

  // the known parallactic angle rotation of the feeds
  Horizon horizon;
  Calibration::Parallactic parallactic;
  parallactic.set_directional( &horizon );

  Fixed fixed;
  Calibration::CelestialProjection projection;
  projection.set_projection( &fixed );

  Mount* mount = &horizon;
  if (test_projection)
    mount = &fixed;

  // Parkes-like
  mount->set_observatory_latitude (-33 * M_PI/180.0);
  mount->set_observatory_longitude (0);

  // 0437-like
  sky_coord position ("00:00-47:15");
  mount->set_source_coordinates (position);

  cerr << "Generating " << nloop << " random source and receiver combinations"
    " (difficulty=" << difficulty << ")" << endl;

  cerr << "Simulating " << nobs << " observations of " 
       << nstates << " source states and " << ncal 
       << " calibrator observation" << endl;

  if (ncoef)
    cerr << "Including a backend with " << ncoef << " polynomial coefficients" << endl;

  cerr << "Hour angle ranges from " << ha_min << " to " << ha_max << " hours" << endl;

  ha_min *= M_PI/12.0;
  ha_max *= M_PI/12.0;
    
  unsigned errors = 0;
  unsigned reported_errors = 0;

  int reported_finished = -1;

  for (unsigned i=0; i<nloop; i++) try
  {
    if (vverbose)
      cerr << "runtest " << i << endl;

    if (test_projection)
    {
      if (runtest (projection, i) < 0)
        errors ++;
    }
    else
    {
      if (runtest (parallactic, i) < 0)
        errors ++;
    }

    int finished = (i*100)/nloop;

    if ((finished != reported_finished) || (errors != reported_errors))
    {
      cerr << "Finished: " << finished << "% -- errors: " << errors << 
        " (" << float((errors*1000)/(i+1))/10.0 << "%)" << endl;

      reported_finished = finished;
      reported_errors = errors;
    }

    if (!(degenerate_V_rotation || degenerate_V_boost))
      hamaker = !hamaker;
  }
  catch (Error& error)
  {
    cerr << "Error " << error << endl;
    errors ++;
  }

  cerr << "count Hamaker=" << count_Hamaker << " Britton=" << count_Britton << endl;

  log_cond_Hamaker /= count_Hamaker;
  log_cond_Britton /= count_Britton;

  cerr << "mean log(cond(Hessian) Hamaker=" << log_cond_Hamaker << " Britton=" << log_cond_Britton << endl;

  float percent = ((errors*1000)/nloop)/10.0;

  if (errors)
  {
    cerr << "Failed " << errors << " out of " << nloop << " times" 
            " (" << percent << "%)" << endl;

    if (percent > 5)
      return -1;
    else
      cerr << "OK" << endl;
  }

  if (test_GIC)
  {
    mean_AIC /= nloop;
    mean_GIC /= nloop;
    mean_SIC /= nloop;
    
    cout << "delta AIC min=" << min_AIC << "\t max=" << max_AIC << "\t mean=" << mean_AIC << endl;
    cout << "delta GIC min=" << min_GIC << "\t max=" << max_GIC << "\t mean=" << mean_GIC << endl;
    cout << "delta SIC min=" << min_SIC << "\t max=" << max_SIC << "\t mean=" << mean_SIC << endl;
  }

  return 0;
}

void random_receiver_old (Jones<double>& receiver)
{
  random_vector (receiver, 10.0);
  receiver /= sqrt(det(receiver));
}

