#include <math.h>

#include "Pulsar/Profile.h"
#include "Pulsar/Calculator.h"
#include "Error.h"
#include "Estimate.h"

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

#include "interpolate.h"
#include "model_profile.h"

// The WINDOW size is used to set the region around the point of 
// maximum correlation that will be used in the interpolation.
// The find_peak_edges routine seems to get it wrong when the S/N 
// is low, this simple method is more robust.

// Experimentation with test_gtd.C suggests that the following value
// works best in most situations:

#define WINDOW 0.0125

void wrap (int& binval, int nbin) {
  if (binval < 0)
    binval += nbin;
  else if (binval > nbin-1)
    binval -= nbin;
}

double Pulsar::Profile::GaussianShift (const Profile& std, float& ephase, 
				       vector<float>& corr, 
				       MEAL::Gaussian& model,
				       int& rise, int& fall, int& ofs, 
				       bool store) const
{
  // This algorithm interpolates the time domain cross correlation
  // function by fitting a Gaussian.

  bool verbose = false;

  // First compute the standard cross correlation function:

  Reference::To<Pulsar::Profile> ptr = clone();
  Reference::To<Pulsar::Profile> stp = std.clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  if (store) {
    for (unsigned i = 0; i < ptr->get_nbin(); i++)
      corr.push_back(ptr->get_amps()[i]);
  }
  
  // Find the peak (can be done a number of ways, this is the simplest)

  int brise = 0;
  int bfall = 0;
  
  int binmax = ptr->find_max_bin();
  int offset = 0;

  brise = binmax - int(WINDOW*float(ptr->get_nbin()));
  bfall = binmax + int(WINDOW*float(ptr->get_nbin()));

  if (brise < bfall) {
    offset = int((ptr->get_nbin())/2.0) - binmax;
  }
  else {
    offset = binmax - int((ptr->get_nbin())/2.0);
  }
  
  brise  += offset;
  bfall  += offset;
  binmax += offset;
    
  wrap(brise,  ptr->get_nbin());
  wrap(bfall,  ptr->get_nbin());
  wrap(binmax, ptr->get_nbin());

  if (store) {
    rise = brise;
    fall = bfall;
    ofs  = offset;
  }

  try{
    
    float threshold = 0.001;
    
    MEAL::Gaussian gm;
    
    gm.set_centre(binmax);
    gm.set_width(abs(bfall - brise));
    gm.set_height(ptr->max());
    gm.set_cyclic(false);
    
    MEAL::Axis<double> argument;
    gm.set_argument (0, &argument);
    
    vector< MEAL::Axis<double>::Value > data_x;  // x-ordinate of data
    vector< Estimate<double> > data_y;       // y-ordinate of data with error
    
    int index = 0;

    for (int i = brise; i < bfall; i++) {
      data_x.push_back ( argument.get_Value(double(i)) );
      index = i;
      index -= offset;
      wrap(index,  ptr->get_nbin());
      data_y.push_back( Estimate<double>(ptr->get_amps()[index], 
					 ptr->get_amps()[index]/50.0) );
    }
    
    MEAL::LevenbergMarquardt<double> fit;
    fit.verbose = MEAL::Function::verbose;
    
    float chisq = fit.init (data_x, data_y, gm);
    if (verbose)
      cerr << "initial chisq = " << chisq << endl;
    
    unsigned iter = 1;
    unsigned not_improving = 0;
    while (not_improving < 25) {
      if (verbose)
	cerr << "iteration " << iter << endl;
      float nchisq = fit.iter (data_x, data_y, gm);
      if (verbose)
	cerr << "     chisq = " << nchisq << endl;
      
      if (nchisq < chisq) {
	float diffchisq = chisq - nchisq;
	chisq = nchisq;
	not_improving = 0;
	if (diffchisq/chisq < threshold && diffchisq > 0) {
	  if (verbose)
	    cerr << "No big diff in chisq = " << diffchisq << endl;
	  break;
	}
      }
      else
	not_improving ++;
      
      iter ++;
    }
    
    double free_parms = data_x.size() + gm.get_nparam();

    if (verbose)
      cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
	   << chisq / free_parms << endl;
    
    if (store) {
      model = gm;
    }
    
    double shift = (gm.get_centre() - double(offset)) / 
      double(ptr->get_nbin());

    // Wrap the result so it is always withing +/- 0.5 phase units

    if (shift < -0.5)
      shift += 1.0;
    else if (shift > 0.5)
      shift -= 1.0;

    // Compute the error based on how far you have to move the
    // centre of the Gaussian before the chi-sq doubles

    float aim = chisq * 2.0;
    float cst = chisq;

    double raw = gm.get_centre();
    double itr = 0.0;

    while (cst < aim) {
      gm.set_centre(raw + itr);
      cst = fit.init(data_x, data_y, gm);
      itr += 0.01;
    }

    ephase = (itr * 2.0) / double(ptr->get_nbin());

    return shift;
  }
  catch (Error& error) {
    cerr << error << endl;
    ephase = 0.5;
    return (0.0);
  }
}

double Pulsar::Profile::ZeroPadShift (const Profile& std, 
				      float& ephase, vector<float>& corr,
				      vector<float>& interp, bool store) const
{
  // This algorithm uses zero padding in the fourier domain to
  // interpolate the cross correlation function in the time
  // domain. The technique is described at:
  //
  // http://www.dspguru.com/howto/tech/zeropad2.htm

  // First compute the standard cross correlation function:

  Reference::To<Pulsar::Profile> ptr = clone();
  Reference::To<Pulsar::Profile> stp = std.clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  vector< Estimate<float> > correlation;
  
  for (unsigned i = 0; i < get_nbin(); i++) {
    correlation.push_back(ptr->get_amps()[i]);
    if (store)
      corr.push_back(ptr->get_amps()[i]);
  }
  
  vector< Estimate<float> > interpolated;
  
  interpolated.resize(correlation.size() * Pulsar::Profile::ZPSF);
  
  // Perform the zero-pad interpolation
  
  fft::interpolate(interpolated, correlation);
  
  // Find the peak of the correlation function
  
  float maxval = 0.0;
  float maxloc = 0.0;
  
  for (unsigned i = 0; i < interpolated.size(); i++) {
    if (store)
      interp.push_back(interpolated[i].val);
    if (interpolated[i].val > maxval) {
      maxval = interpolated[i].val;
      maxloc = float(i) / Pulsar::Profile::ZPSF;
    }
  }
  
  // Error estimate (???)
  ephase = 1.0 / float(get_nbin());
  
  double shift = double(maxloc) / double(get_nbin());
  
  if (shift < -0.5)
    shift += 1.0;
  else if (shift > 0.5)
    shift -= 1.0;
  
  return shift;
}

double Pulsar::Profile::PhaseGradShift (const Profile& std, float& ephase,
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
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "standard DC=%lf > max float", stdcopy.sum());

  if (prfcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "profile DC=%lf > max float", stdcopy.sum());

  if (verbose)
    cerr << "Profile::PhaseGradShift compare nbin="<< nbin 
	 <<" "<< stdcopy.nbin <<endl;

  if (nbin > stdcopy.nbin) {
    if (nbin % stdcopy.nbin)
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = nbin / stdcopy.nbin;
    prfcopy.bscrunch (nscrunch);
  }

  if (nbin < stdcopy.nbin) {
    if (stdcopy.nbin % nbin)
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d", nbin, stdcopy.nbin);

    unsigned nscrunch = stdcopy.nbin / nbin;
    stdcopy.bscrunch (nscrunch);
  }

  double shift = 0.0;
  float eshift = 0.0;

  prfcopy.fftconv (stdcopy, shift, eshift, snrfft, esnrfft);
  if (ephase) {
    ephase = eshift / float(stdcopy.nbin);
  }
  return shift / double(stdcopy.nbin);
}

void Pulsar::Profile::fftconv (Profile& std, 
			       double& shift, float& eshift, 
			       float& snrfft, float& esnrfft)
{
  double scale, sigma_scale;
  double dshift, sigma_dshift;
  double chisq;
  
  int ret=0;


  if (!Profile::legacy)
    ret = model_profile (nbin, 1, &amps, &(std.amps), &scale, &sigma_scale, 
			   &dshift, &sigma_dshift, &chisq, verbose);
  else
    ret = legacy_fftconv (nbin, &amps[0], &(std.amps[0]),
			  &dshift, &sigma_dshift, &snrfft, &esnrfft);
  
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
  if (!Pulsar::Profile::legacy) {
    snrfft = 2 * sqrt( float(nbin) ) * scale / rms;
    
    esnrfft = snrfft * eshift / scale;
  }
}
