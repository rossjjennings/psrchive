/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include "Pulsar/SimPolnTiming.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/PolnProfileFitAnalysis.h"
#include "Pulsar/PhaseGradShift.h"

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Instrument.h"
#include "Pulsar/VariableBackend.h"

#include "model_profile.h"

#include "MEAL/Polar.h"
#include "BoxMuller.h"
#include "random.h"

#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#endif

#include <iostream>

using namespace std;

static BoxMuller gasdev (usec_seed());


Pulsar::Range::Range (unsigned _steps, double _min, double _max)
{
  steps = _steps;
  min = _min;
  max = _max;

  start = 0;
  stop = steps;
}

//! Get the current value in the range
double Pulsar::Range::get_value (unsigned current)
{
  return min + (max-min)*double(current)/(steps-1);
}

//! Parse the range from a string
void Pulsar::Range::parse (const string& txt)
{
  int c;
  c = sscanf (txt.c_str(), "%u:%lf,%lf:%u,%u", &steps, &min,&max, &start,&stop);
  switch (c) {
  case 3:
    start = 0;
  case 4:
    stop = steps;
  case 5:
    return;
  }

  c = sscanf (txt.c_str(), "%u::%u,%u", &steps,&start,&stop);
  switch (c) {
  case 1:
    start = 0;
  case 2:
    stop = steps;
  case 3:
    return;
  }

  throw Error (InvalidParam, "Pulsar::Range::parse",
	       "Could not parse range from '%s'", txt.c_str());
}

std::ostream& Pulsar::operator<< (std::ostream& ostr, const Range& r)
{ 
  return ostr << "from " << r.min << " to " << r.max << " in " << r.steps
	      << " steps " << " (start " << r.start 
	      << " stop " << r.stop << ")";
}

Pulsar::SimPolnTiming::SimPolnTiming () :
  slope (1, 1.5, 25.5),
  width (1, 0.01, 0.21),
  dgain (1, 0.005, 0.05),
  parallactic (1, -90, +90),
  boost_vector (1, -M_PI, M_PI)
{
  // generate gaussian total intensity profiles
  generate.set_total_intensity (&gaussian);

  gaussian.set_width  (0.02*M_PI);
  gaussian.set_height (1.0);
  gaussian.set_period (2*M_PI);

  // generate linear polarization according to RVM
  generate.set_position_angle (&rvm);

  rvm.line_of_sight->set_value (83.0*M_PI/180);
  rvm.magnetic_axis->set_value (89.0*M_PI/180);
  rvm.reference_position_angle->set_value (M_PI/4);

  set_centre (M_PI);

  // set degree of polarization
  generate.set_degree (0.4);

  iterations = 1;
  nbin = 256;

  measurement_noise = 0.01;
  standard_noise = 0.0;

  offset = 0.0;

  rvm_beta = 0.0;

  vary_line_of_sight = false;

  poln_slope = 0;
  diff_gain = 0.0;

  parallactic_angle = 0.0;

  B_orientation = 0;
  B_ellipticity = 0;
  beta = 0.0;

  as_Estimate = false;
  logarithmic = false;

  output = &cout;
}

Pulsar::SimPolnTiming::~SimPolnTiming ()
{
}

//! Set the phase of the standard profile centre
void Pulsar::SimPolnTiming::set_centre (double centre)
{
  gaussian.set_centre (centre);
  rvm.magnetic_meridian->set_value (centre);
}


void Pulsar::SimPolnTiming::set_standard (Archive* _archive)
{
  archive = _archive;

  if (!archive)
    return;

  if (archive->get_type() != Signal::Pulsar)
    cerr << "pts warning: " << archive->get_filename()
	 << " is not a pulsar archive" << endl;
  
  archive->dedisperse ();
  archive->fscrunch ();
  archive->tscrunch ();
  archive->convert_state (Signal::Stokes);
  archive->remove_baseline ();

  title = archive->get_source ();

  cerr << "Standard set to " << title << endl;
}


//! Run the simulation, outputting results to output
void Pulsar::SimPolnTiming::run_simulation ()
{
  one_loop (3);
}

//! One loop in the simulation
void Pulsar::SimPolnTiming::one_loop (unsigned level)
{
  switch (level) {

  case 1: {

    MeanEstimate<double> m;

    for (unsigned ibeta=dgain.start; ibeta < dgain.stop; ibeta++)
    {

      if (dgain.steps > 1)
	diff_gain = dgain.get_value (ibeta);

      cerr << "Pulsar::SimPolnTiming::one_loop call one_step" << endl;
      Result result = one_step ();

      m += result.I_mean / diff_gain;

    }

    if (diff_gain) {
      *output << "DIFF_GAIN: slope= " << poln_slope 
	   << " width= " << gaussian.get_width() / (2*M_PI);
      if (as_Estimate)
	*output << " m= " << m << endl;
      else
	*output << " m= " << m.get_Estimate().get_value() 
		<< " merr= " << m.get_Estimate().get_error() << endl;
    }

    break;

  }

  case 2: {

    for (unsigned icos=slope.start; icos < slope.stop; icos++) {
      
      if (slope.steps > 1) {

	poln_slope = slope.get_value (icos);

	double cos_angle = 1.0 / poln_slope;
	double angle = acos (cos_angle);
	cerr << "angle=" << angle*180/M_PI << "deg" << endl;
	
	if (vary_line_of_sight)
	  rvm.line_of_sight->set_value (angle);
	else
	  rvm.magnetic_axis->set_value (angle);
	
	cerr << "tan(angle)=" << tan(angle) << endl;
	cerr << "csc(angle)=" << 1.0/cos(angle) << endl;
      }
      
      one_loop (level-1);
    }

    break;

  }

  case 3: {

    for (unsigned iw=width.start; iw < width.stop; iw++) {

      if (width.steps > 1)
	gaussian.set_width  (width.get_value(iw)*2*M_PI);

      one_loop (level-1);

    }

    break;

  }

  default:
    throw Error (InvalidState, "Pulsar::SimPolnTiming::one_loop",
		 "invalid level = %d", level);

  }

}

#define SQR(x) ((x)*(x))


//! Account for noise in the standard profile
void Pulsar::SimPolnTiming::susceptibility ()
{
  if (parallactic.steps < 1)
    return;

  double S_max = 0;
  double I_max = 0;

  double period = 1.0;

  if (archive)
    period = archive->get_Integration(0)->get_folding_period ();

  *output << "Period: " << period << " seconds" << endl;

  for (unsigned ipa=parallactic.start; ipa < parallactic.stop; ipa++) {
      
    parallactic_angle = parallactic.get_value (ipa);
    
    cerr << "Pulsar::SimPolnTiming::susceptibility call one_step" << endl;
    Result result = one_step ();

    if (iterations == 1) {
      result.S_mean = result.S_est_mean;
      result.I_mean = result.I_est_mean;
    }

    *output << "PARA= " << parallactic_angle;

    if (archive) {
      result.S_mean *= period * 1e9; // ns
      result.I_mean *= period * 1e9;
    }

    if (as_Estimate)
      *output << " S= " << result.S_mean << " I= " << result.I_mean << endl;
    else
      *output << " S= " << result.S_mean.get_value()
	      << " S_err= " << result.S_mean.get_error()
	      << " I= " << result.I_mean.get_value()
	      << " I_err= " << result.I_mean.get_error() << endl;

    double S_norm = fabs(result.S_mean.get_value())/result.S_mean.get_error();
    if (S_norm > S_max)
      S_max = S_norm;

    double I_norm = fabs(result.I_mean.get_value())/result.I_mean.get_error();
    if (I_norm > I_max)
      I_max = I_norm;

  }

}








//! Account for noise in the standard profile
void Pulsar::SimPolnTiming::boost_around ()
{
  if (boost_vector.steps < 1)
    return;

  if (!beta)
    throw Error (InvalidState, "Pulsar::SimPolnTiming::boost_around",
		 "about to boost around without any boost");

  double period = 1.0;

  if (archive)
    period = archive->get_Integration(0)->get_folding_period ();

  *output << "Period: " << period << " seconds" << endl;

  for (unsigned io=boost_vector.start; io < boost_vector.stop; io++) {
      
    B_orientation = boost_vector.get_value (io);
    
    for (unsigned ie=boost_vector.start/2; ie < boost_vector.stop/2; ie++) {

      B_ellipticity = 0.5 * boost_vector.get_value (ie*2);

      Result result = one_step ();

      if (iterations == 1) {
	result.S_mean = result.S_est_mean;
	result.I_mean = result.I_est_mean;
      }

      *output << "BOOST o= " << B_orientation << " e= " << B_ellipticity;

      if (archive) {
	result.S_mean *= period * 1e9; // ns
	result.I_mean *= period * 1e9;
      }

      if (as_Estimate)
	*output << " S= " << result.S_mean << " I= " << result.I_mean << endl;
      else
	*output << " S= " << result.S_mean.get_value()
		<< " S_err= " << result.S_mean.get_error()
		<< " I= " << result.I_mean.get_value()
		<< " I_err= " << result.I_mean.get_error() << endl;

    }
  }
}




















//! Account for noise in the standard profile
void Pulsar::SimPolnTiming::extrapolate_benefit (unsigned steps)
{
  if (steps < 2)
    return;

  double noise_scale = 0.001;
  double intrinsic_noise = 0;

  if (archive) {

    GaussianBaseline finder;
    finder.set_Profile (archive->get_Profile(0,0,0));

    PhaseWeight mask;
    finder.get_weight (&mask);

    unsigned npol = archive->get_npol();

    cerr << "baseline noise in " << npol << " polns" << endl;
    MeanEstimate<double> mean_noise;

    for (unsigned ipol=0; ipol < npol; ipol++) {
      mask.set_Profile( archive->get_Profile(0,ipol,0) );

      Estimate<double> rms = sqrt(mask.get_variance());

      cerr << "noise in ipol=" << ipol << " " << rms << endl;
      mean_noise += rms;
    }

    intrinsic_noise = mean_noise.get_Estimate().get_value();
    noise_scale = intrinsic_noise;

    *output << "STANDARD NOISE= " << mean_noise << endl;

    double max_val = archive->get_Profile(0,0,0)->max();
    *output << "STANDARD MAX= " << max_val << endl;

    measurement_noise = 3.0 * intrinsic_noise;
      
  }

  standard_noise = 0;

  for (unsigned ibeta=0; ibeta < steps; ibeta++) {

    MeanEstimate<double> I_err, S_err;

    for (unsigned iter=0; iter < iterations; iter++) {

      Result result = one_step ();
      I_err += result.I_err;
      S_err += result.S_err;

    }

    Estimate<double> benefit = I_err.get_Estimate() / S_err.get_Estimate();

    double total_noise = sqrt ( SQR(standard_noise) + SQR(intrinsic_noise) );
    *output << "STDRMS= " << total_noise << " benefit= ";

    if (as_Estimate)
      *output << benefit << endl;
    else
      *output << benefit.get_value() <<
	" benefit_err= " << benefit.get_error() << endl;

    if (logarithmic)
      standard_noise = pow(2.0, double(ibeta+1)) * noise_scale;
    else
      standard_noise += 0.5 * noise_scale;

  }

}

const static double rdeg = 180/M_PI;
static long idum = -1;

Estimate<double> variance (const vector<float>& vals, const string& label,
			   Estimate<double>& mean);

template<typename T>
class MeanVariance {

public:
  MeanVariance () { sum=0.0; sumsq=0.0; count=0; }
  MeanVariance& operator+= (T val)
  { sum+=val; sumsq+=val*val; count++; return *this; }
  Estimate<T> get_Estimate() const
  {
    T mean=sum/count;
    return Estimate<T>(mean,sumsq/count-mean*mean);
  }
protected:
  T sum, sumsq;
  unsigned count;
};


Vector<3,double> unit_vector (double theta, double phi)
{
  double cos_theta = cos(theta);
  double sin_theta = sin(theta);
  double cos_phi = cos(phi);
  double sin_phi = sin(phi);

  return Vector<3,double> (cos_theta*cos_phi, sin_theta*cos_phi, sin_phi);
}

//! One step in the simulation
Pulsar::SimPolnTiming::Result Pulsar::SimPolnTiming::one_step ()
{
  if (rvm_beta)
    rvm.line_of_sight->set_value (rvm.magnetic_axis->get_value() + rvm_beta);

  //! Scalar tempalte matching algorithm
  PhaseGradShift stm;

  //! Matrix template matching algorithm
  PolnProfileFit fit;

  Reference::To<PolnProfile> standard;

  if (archive) {

    cerr << "Using provided standard" << endl;
    standard = archive->get_Integration(0)->new_PolnProfile(0);
    nbin = archive->get_nbin();

  }
  else {

    cerr <<
      "RVM parameters:"
      "\n Reference P.A.: " << rvm.reference_position_angle->get_value()*rdeg
	 << "\n Line of sight: " << rvm.line_of_sight->get_value()*rdeg
	 << "\n Magnetic axis: " << rvm.magnetic_axis->get_value()*rdeg
	 << "\n Magnetic meridian: " << rvm.magnetic_meridian->get_value()*rdeg
	 << endl;
	
    generate.set_noise_variance (0.0);
    standard = generate.get_PolnProfile (nbin);

  }

  if (standard_noise)
  {
    cerr << "Adding noise (sigma=" << standard_noise << ") to standard" <<endl;
    standard = standard->clone();
    for (unsigned ipol=0; ipol < 4; ipol++)
    {
      float* amps = standard->get_Profile(ipol)->get_amps();
      for (unsigned ibin=0; ibin < nbin; ibin++)
	amps[ibin] += standard_noise * gasdev();
    }
  }

  cerr << "Set MTM standard" << endl;
  fit.set_standard( standard );

  cerr << "Set STM standard" << endl;
  stm.set_standard( standard->get_Profile(0) );

#ifdef HAVE_PGPLOT_UPDATED

  Plotter plot;
  cpgpage();
  cerr << "Plotting standard" << endl;
  plot.Manchester (standard, false, title.c_str());
  cpgpage();
  plot.fourier (standard);

  if (standard->get_state() == Signal::Stokes)
    cerr << "standard is Stokes" << endl;
  else
    cerr << "standard is not Stokes" << endl;
      
  for (unsigned ipol=0; ipol < 4; ipol++) {
    plot.set_pol (ipol);
    cpgpage();
    plot.plot (standard->get_Profile(ipol));
  }
 
#endif
      
  Result result;

  if (!iterations)
    return result;
      
  // set up to generate observations
  //generate.set_noise_variance (measurement_noise*measurement_noise);
  //gaussian.set_centre (std_centre+offset);
  //rvm.set_magnetic_meridian (std_centre+offset);

  // generate observations
      
  vector<float> S_shift (iterations);
  vector<float> I_shift (iterations);
 
  // the normalized residuals of the instrumental parameter estimates
  vector<double> J_chisq (7, 0.0);

  unsigned count = 0;
      
  MeanEstimate<double> S_mean;
  MeanEstimate<double> I_mean;

  MeanVariance<double> S_chisq;
  MeanVariance<double> I_chisq;

  MeanVariance<double> S0_err;
  MeanVariance<double> S1_err;

  for (unsigned iter=0; iter < iterations; iter++) {

    Reference::To<MEAL::Complex2> J;
    bool xform = false;

    if (diff_gain || parallactic_angle) {

      Calibration::Instrument* inst = new Calibration::Instrument;
      J = inst;

      if (diff_gain) {
	double beta = 0.5 * log (1+diff_gain);
	cerr << "Setting beta=" << beta << endl;
	inst->get_backend()->set_diff_gain (beta);
	xform = true;
      }
      
      if (parallactic_angle) {
	cerr << "Setting PA=" << parallactic_angle << endl;
	inst->set_orientation (0, parallactic_angle*M_PI/180);
	inst->set_orientation (1, parallactic_angle*M_PI/180);
	xform = true;
      }

    }
    else {

      MEAL::Polar* polar = new MEAL::Polar;
      J = polar;

      if (beta) {

	Vector<3,double> bvec;
	bvec = sinh(beta) * unit_vector (B_orientation, B_ellipticity);

	cerr << "boost vector=" << bvec << endl;

	for (unsigned i=0; i<3; i++)
	  polar->set_boost (i, bvec[i]);

	xform = true;

      }

    }

    cerr << "Set transformation" << endl;

    // reset the model for each iteration
    fit.set_transformation( J );

    if (iterations == 1) 
    {
      PolnProfileFit::Analysis analysis;
      analysis.set_fit (&fit);
    }

    cerr << "Clone standard" << endl;

    // generate a noisy profile
    Reference::To<PolnProfile> profile = standard->clone();

    if (xform)
      profile->transform (J->evaluate());

    if (standard_noise)
      measurement_noise = 3.0*standard_noise;

    if (measurement_noise)
    {
      cerr << "Adding noise (sigma=" << measurement_noise << ") to obs" <<endl;
      for (unsigned ipol=0; ipol < 4; ipol++)
      {
	float* amps = profile->get_Profile(ipol)->get_amps();
	for (unsigned ibin=0; ibin < nbin; ibin++)
	  amps[ibin] += measurement_noise * gasdev();
      }
    }

#ifdef HAVE_PGPLOT_UPDATED
    if (iter == 0) {
      cpgpage();
      cerr << "Plotting first observation" << endl;
      plot.Manchester (profile, false, title.c_str());
      cpgpage();
      plot.fourier (profile);
    }
#endif

#if MEASURE_CONDITIONAL_VARIANCE
    // calculate conditional shift
    try {
      for (unsigned i=0; i<J->get_nparam(); i++)
	J->set_infit (i, false);
      fit.fit (profile);
      for (unsigned i=0; i<J->get_nparam(); i++)
	J->set_infit (i, true);
    }
    catch (Error& e) {
      cerr << e << endl;
      continue;
    }

    Estimate<double> shift2 = fit.get_phase ();
    shift2 -= offset/(2*M_PI);
    S0_err += shift2.get_error();
#endif

    // calculate shift using polarimetric fit
    try {
      fit.fit (profile);
    }
    catch (Error& e) {
      cerr << e << endl;
      continue;
    }
	
    Estimate<double> shift = fit.get_phase ();
    shift -= offset/(2*M_PI);

#if MEASURE_CONDITIONAL_VARIANCE
    S1_err += shift.get_error();
    cerr << "Conditional efac=" << shift.get_error() / shift2.get_error()
	 << endl;
#endif



    S_mean += shift;
    S_shift[count] = shift.get_value();
    S_chisq += shift.get_value()*shift.get_value() / shift.get_variance();

    double gain_error = J->get_param(0) - 1.0;
    J_chisq[0] += gain_error*gain_error/J->get_variance(0);
    for (unsigned ip=1; ip < 7; ip++)
      J_chisq[ip] += J->get_param(ip)*J->get_param(ip)/J->get_variance(ip);

    // calculate shift using old method
    if (fit.choose_maximum_harmonic)
      Pulsar::max_harmonic = fit.get_maximum_harmonic();

    stm.set_observation (profile->get_Profile(0));

    shift = stm.get_shift ();
    shift -= offset/(2*M_PI);

    I_mean += shift;
    I_shift[count] = shift.get_value();
    I_chisq += shift.get_value()*shift.get_value() / shift.get_variance();
	
    count ++;



  }

  for (unsigned ip=0; ip < 7; ip++) {
    J_chisq[ip] /= count;
    cerr << "J_chisq[" << ip << "]=" << J_chisq[ip] << endl;
  }

  result.S_est_mean = S_mean.get_Estimate();
  result.I_est_mean = I_mean.get_Estimate();

  *output << "RESULT:" <<
    " slope= " << poln_slope <<
    " width= " << gaussian.get_width() / (2*M_PI) <<
    " dgain= " << diff_gain <<
    " stdrms= " << standard_noise << 
    " S= " << result.S_est_mean.get_value() <<
    " Serr= " << result.S_est_mean.get_error() << 
    " I= " << result.I_est_mean.get_value() <<
    " Ierr= " << result.I_est_mean.get_error() <<
    " Schisq= " << S_chisq.get_Estimate() <<
    " Ichisq= " << I_chisq.get_Estimate();

  if (count < 4) {
    *output << endl;
    return result;
  }

  S_shift.resize(count);
  I_shift.resize(count);
      
  // CALL HERE
  Estimate<double> S_var = variance (S_shift, "Polarimetric Shift",
				     result.S_mean);

  Estimate<double> I_var = variance (I_shift, "Intensity Shift",
				     result.I_mean);

  result.S_err = sqrt(S_var);
  result.I_err = sqrt(I_var);

  *output << " varS= " << result.S_err
	 << " varI= " << result.I_err
	 << " ratio= " << sqrt(I_var/S_var) << endl;

#if 0
  I_var = variance (gain, "Gain", I_var);
  I_var = variance (bst_q, "Diff. Gain", I_var);
  I_var = variance (rot_q, "Diff. Phase", I_var);
  I_var = variance (ori_0, "Orientation 0", I_var);
  I_var = variance (ell_0, "Ellipticity 0", I_var);
  I_var = variance (ori_1, "Orientation 1", I_var);
  I_var = variance (ell_1, "Ellipticity 1", I_var);
#endif

  return result;
}

Estimate<double> variance (const vector<float>& vals, const string& label,
			   Estimate<double>& mean)
{
  unsigned i, n = vals.size();
  double mu1 = 0;
  double mu2 = 0;
  double mu4 = 0;

  // find the mean
  for (i=0; i<n; i++)
    mu1 += vals[i];
  mu1 /= n;

  //cerr << "mean =" << mu1 << endl;

  // find the variance (and its variance)
  for (i=0; i<n; i++) {
    double val = vals[i] - mu1;

    //cerr << "val=" << val << endl;

    val *= val;
    mu2 += val;

    val *= val;
    mu4 += val;
  }

  //cerr << "mu2=" << mu2 << " mu4=" << mu4 << endl;

  // unbiased estimator of variance
  mu2 /= (n-1);
  mu4 /= n;

  double N = n;

  // sample variance
  double var = (N-1)/N * mu2;
  // variance of sample variance
  double var_var = ((N-1)*(N-1)*mu4 - (N-1)*(N-3)*mu2*mu2) / (N*N*N);

  mean.set_value (mu1);
  mean.set_variance (var/double(N));

#ifdef HAVE_PGPLOT
    
  cpgpage ();
  double norm = N / (2*sqrt(2*M_PI));
  cerr << "normalization: " << norm << endl;
  
  double rms = sqrt(var);

  //cpgsvp (.15, .9, .2, .9);
  cpgswin (-3*rms, 3*rms, 0, norm*1.5);
  cpgbox ("binst", 0,0, "bcnst", 0,0);
  cpgbox ("cst", 0,0, "", 0,0);
  
  cpghist (vals.size(), &(vals[0]), -3*rms, 3*rms, 21, 1);
  cpglab (label.c_str(), "Count", "");

#endif

  return Estimate<double> (var,var_var);

}
