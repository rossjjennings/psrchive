#include <math.h>

#include "Pulsar/Profile.h"
#include "Error.h"

#include "model_profile.h"

#include "Gaussian.h"
#include "GaussianModel.h"
#include "LevenbergMarquardt.h"

/*! Calculates the shift relative to a standard using a time domain
  convolution and gaussian fit, returning a Tempo::toa object */
Tempo::toa Pulsar::Profile::time_domain_toa (const Profile& std, const MJD& mjd, 
					     double period, char nsite) const
{
  vector<double> convolution;
  vector<double> indices;
  int nbin = get_nbin();
  for (int i = -1*(nbin/2); i < nbin/2; i++) {
    convolution.push_back(tdl_convolve(&std, i));
    indices.push_back(double(i));
  }

  // Fit a gaussian to the convolution function

  // Three parameters for the gaussian:

  // Midpoint
  // Height
  // Width

  vector<double> params;
  params.resize(3);
  params[1] = 0.0;

  for (unsigned i = 0; i < convolution.size(); i++) {
    if (convolution[i] > params[1]) {
      params[1] = convolution[i];
      params[0] = indices[i];
    }
  }
  
  params[2] = double(nbin)/8.0;

  // The weights array (set to a uniform value)
  vector<double> weights;
  
  for (unsigned i = 0; i < convolution.size(); i++) {
    weights.push_back(params[1] / 20.0);
  }

  // A vector to hold the solution
  vector<double> ans;
  ans.resize(convolution.size());
  
  Numerical::GaussianModel model (params);
  model.set_fitall(true);
  model.gaussians[0].cyclic=false;
  model.fill (indices,ans);

  Numerical::LevenbergMarquardt<double> fit;
  float chisq = fit.init (indices, convolution, weights, model);
  int iter = 1;
  while (iter < 100) {
    float nchisq = fit.iter (indices, convolution, weights, model);
                      
    if (nchisq < chisq) {
      chisq = nchisq;
    }                                                                                
    iter ++;
  }

  // Now ans should contain the fitted points in the gaussian

  double bin_time = period / double(nbin);
  double offset = model.gaussians[0].centre * bin_time;
  double error = model.gaussians[0].sigma * bin_time;

  Tempo::toa retval (Tempo::toa::Parkes);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + offset);
  retval.set_error     (error * 1e6);

  retval.set_telescope (nsite);
  
  return retval;
}

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa Pulsar::Profile::toa (const Profile& std, const MJD& mjd, 
				 double period, char nsite) const
{
  Tempo::toa retval (Tempo::toa::Parkes);

  float ephase, snrfft, esnrfft; 
  double phase = shift (std, ephase, snrfft, esnrfft);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + phase * period);
  retval.set_error     (ephase * period * 1e6);

  retval.set_telescope (nsite);

  if (verbose) {
    fprintf (stderr, "Pulsar::Profile::toa created:\n");
    retval.unload (stderr);
  }
  return retval;
}


double Pulsar::Profile::shift (const Profile& std, float& ephase,
			       float& snrfft, float& esnrfft) const 
{
  Profile stdcopy = std;
  Profile prfcopy = *this;

  // set these in case something goes wrong
  ephase = 999;
  snrfft = 0;
  esnrfft = 999;

  // max float is of order 10^{38} - check that we won't exceed this
  // limiting factor in the DC term of the fourier transform

  if (stdcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::shift", 
		 "standard DC=%lf > max float", stdcopy.sum());

  if (prfcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::shift", 
		 "profile DC=%lf > max float", stdcopy.sum());

  if (verbose)
    cerr << "Profile::shift compare nbin="<< nbin <<" "<< stdcopy.nbin <<endl;

  if (nbin > stdcopy.nbin) {
    if (nbin % stdcopy.nbin)
      throw Error (InvalidState, "Profile::shift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = nbin / stdcopy.nbin;
    prfcopy.bscrunch (nscrunch);
  }

  if (nbin < stdcopy.nbin) {
    if (stdcopy.nbin % nbin)
      throw Error (InvalidState, "Profile::shift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = stdcopy.nbin / nbin;
    stdcopy.bscrunch (nscrunch);
  }

  double shift = 0.0;
  float eshift = 0.0;

  prfcopy.fftconv (stdcopy, shift, eshift, snrfft, esnrfft);

  if (ephase)
    ephase = eshift / float(stdcopy.nbin);

  return shift / double(stdcopy.nbin);

}


void Pulsar::Profile::fftconv (Profile& std, 
			       double& shift, float& eshift, 
			       float& snrfft, float& esnrfft)
{
  double scale, sigma_scale;
  double dshift, sigma_dshift;
  double chisq;
  
  int ret = model_profile (nbin, 1, &amps, &(std.amps), &scale, &sigma_scale, 
			   &dshift, &sigma_dshift, &chisq, verbose);

  if (ret != 0)
    throw Error (FailedCall, "Profile::fftconv", "model_profile failed");
  
  if (verbose) cerr << "Profile::fftconv"
		 " shift=" << dshift << " bins,"
		 " error=" << sigma_dshift <<
		 " scale=" << scale << 
		 " error=" << sigma_scale <<
		 " chisq=" << chisq << endl;

  double rms = sqrt( chisq / (float(nbin/2)-1.0) );

  shift = dshift;

  eshift = sigma_dshift;

  snrfft = 2 * sqrt( float(nbin) ) * scale / rms;

  esnrfft = snrfft * eshift / scale;
}  

