#include <math.h>

#include "Pulsar/Profile.h"
#include "Error.h"

#include "Calibration/LevenbergMarquardt.h"
#include "Calibration/Polynomial.h"
#include "Calibration/Axis.h"

#include "model_profile.h"

/*! Calculates the shift relative to a standard using a time domain
  correlation to get course discrete values of the correlation function 
  and a gaussian interpolation to find the point of maximum correlation, 
  returning a Tempo::toa object */
Tempo::toa Pulsar::Profile::time_domain_toa (const Profile& std, 
					     const MJD& mjd, 
					     double period, char nsite) const
{
  // The Profile::correlate function creates a profile whose amps
  // are the values of the correlation function, starting at zero
  // lag and increasing

  vector<double> correlation;
  vector<double> lags;

  Reference::To<Pulsar::Profile> ptr = clone();

  // Remove the baseline (done twice to minimise rounding error)
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  // Fit a gaussian to the peak part of the correlation function

  // Find the point of best correlation
  
  double maxpt = ptr->find_max_phase(0.15);

  // Centre on it, to allow the gaussian fit a better 
  // chance of success

  ptr->rotate(0.5-maxpt);

  float phswidth = ptr->width(85.0);
  float binwidth = phswidth * nbin;

  int nbin = get_nbin();
  int maxbin = ptr->find_max_bin();

  int lowbin = int(maxbin-(binwidth/2));
  int hibin  = int(maxbin+(binwidth/2));

  if (lowbin < 0)
    lowbin = 0;

  if (hibin > nbin)
    hibin = nbin;

  for (int i = lowbin; i < hibin; i++) {
    correlation.push_back(ptr->get_amps()[i]);
    lags.push_back(double(i));
  }

  // Construct a polynomial model
  
  Calibration::Polynomial model(3);

  Calibration::Axis<double> argument;
  model.set_argument (0, &argument);

  vector< Calibration::Axis<double>::Value > data_x;  // x-ordinate of data
  vector< Estimate<double> > data_y;       // y-ordinate of data with error

  for (unsigned i = 0; i < correlation.size(); i++) {
    data_x.push_back ( argument.get_Value(lags[i]) );
    data_y.push_back ( Estimate<double>(correlation[i],0.0) );
  }

  Calibration::LevenbergMarquardt<double> fit;
  fit.verbose = Calibration::Model::verbose;

  float chisq = fit.init (data_x, data_y, model);
  cerr << "initial chisq = " << chisq << endl;

  float threshold = 0.001;

  int iter = 1;
  while (iter < 25) {
    cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (data_x, data_y, model);
    cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      iter = 1;
      if (diffchisq/chisq < threshold && diffchisq > 0) {
        cerr << "no big diff in chisq = " << diffchisq << endl;
        break;
      }
    }
    else
      iter ++;
  }

  double free_parms = data_x.size() + model.get_nparam();

  cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
       << chisq / free_parms << endl;

  double bin_time = period / double(nbin);
  double offset = (model.get_param(0) - maxpt*nbin) * bin_time;
  double error = model.get_variance(0) * bin_time;

  Tempo::toa retval (Tempo::toa::Parkes);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + offset);
  retval.set_error     (error);

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

