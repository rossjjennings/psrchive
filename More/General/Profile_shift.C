#include <math.h>

#include "Pulsar/Profile.h"
#include "Pulsar/Calculator.h"
#include "Error.h"
#include "Estimate.h"

#include "interpolate.h"
#include "model_profile.h"
#include "GaussJordan.h"

double Pulsar::Profile::ZeroPadShift (const Profile& std, 
				      float& ephase, vector<float>& corr,
				      vector<float>& interp, bool store) const
{
  float SAMPLE_FACTOR = 4.0;

  // First compute the standard cross correlation with single
  // bin precision:

  Reference::To<Pulsar::Profile> ptr = clone();
  Reference::To<Pulsar::Profile> stp = std.clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  vector< Estimate<float> > correlation;
  correlation.resize(get_nbin());
  
  for (unsigned i = 0; i < get_nbin(); i++) {
    correlation[i].val = ptr->get_amps()[i];
    correlation[i].var = 0.0;
    if (store)
      corr.push_back(ptr->get_amps()[i]);
  }
  
  vector< Estimate<float> > interpolated;
  
  interpolated.resize(correlation.size() * unsigned(SAMPLE_FACTOR));
  
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
      maxloc = float(i) / SAMPLE_FACTOR;
    }
  }
  
  // Error estimate?
  ephase = 1.0 / SAMPLE_FACTOR;
  
  return maxloc / double(get_nbin());
}

double Pulsar::Profile::ParIntShift (const Profile& std, float& error,
				     float* corr, float* fn) const
{
  // The Profile::correlate function creates a profile whose amps
  // are the values of the correlation function, starting at zero
  // lag and increasing

  vector<double> correlation;
  vector<double> lags;

  Reference::To<Pulsar::Profile> ptr = clone();
  Reference::To<Pulsar::Profile> stp = std.clone();

  // Remove the baseline (done twice to minimise rounding error)
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  if (corr != 0) {
    for (unsigned i = 0; i < ptr->get_nbin(); i++) {
      corr[i] = ptr->get_amps()[i];
    }
  }  

  // Find the point of best correlation
  
  int maxbin = ptr->find_max_bin();
  
  double x1,x2,x3;
  double y1,y2,y3;

  x1 = double(maxbin - 1);
  x2 = double(maxbin);
  x3 = double(maxbin + 1);

  y1 = ptr->get_amps()[(maxbin - 1)%get_nbin()];
  y2 = ptr->get_amps()[(maxbin)%get_nbin()];
  y3 = ptr->get_amps()[(maxbin + 1)%get_nbin()];

  x1 /= double(get_nbin());
  x2 /= double(get_nbin());
  x3 /= double(get_nbin());
 
  vector< vector<double> > matrix;
  vector< vector<double> > empty;

  matrix.resize(3);
  for (unsigned i = 0; i < matrix.size(); i++) {
    matrix[i].resize(3);
  }
  
  matrix[0][0] = x1*x1;
  matrix[0][1] = x1;
  matrix[0][2] = 1;

  matrix[1][0] = x2*x2;
  matrix[1][1] = x2;
  matrix[1][2] = 1;

  matrix[2][0] = x3*x3;
  matrix[2][1] = x3;
  matrix[2][2] = 1;

  // Invert the matrix

  Numerical::GaussJordan (matrix, empty, 3);

  // Solve for the coefficients of our parabola, in the form:
  // y = Ax^2 + Bx + C
  
  double A = matrix[0][0]*y1 + matrix[0][1]*y2 + matrix[0][2]*y3;
  double B = matrix[1][0]*y1 + matrix[1][1]*y2 + matrix[1][2]*y3;
  double C = matrix[2][0]*y1 + matrix[2][1]*y2 + matrix[2][2]*y3;

  // Now calculate the equivalent coefficients for the form:
  // y = D - E(x - F)^2
  //
  // Which has the physical interperatation:
  //   D => not much use
  //   E => related to the TOA error
  //   F => the relative shift

  // Use the equations:
  //   A = -E       => E = -A
  //   B = 2EF      => F = -B/2A
  //   C = D - EF^2 => D = C - (B^2/4A)

  double D = C - (B*B/(4.0*A));
  double E = -1.0 * A;
  double F =  (-1.0 * B) / (2.0 * A);

  if (fn != 0) {
    fn[0] = D;
    fn[1] = E;
    fn[2] = F;
  }

  /*

  // This error estimate is somewhat speculative... It is
  // computed as the full width at half maximum of the
  // parabola, assuming the baseline is the average level
  // of the two bins either side the maximum bin in the 
  // correlation function.

  double height = ((y1 + y3) / 2.0);

  if ((D - height)/E < 0.0) {
    throw Error(FailedCall, "Profile::ParIntShift",
		"aborting before floating exception");
  }
  
  // The error in phase units
  error = (float(2.0 * sqrt((D - height)/E)))/2.0;

  // Essentially arbitrary scaling factor
  error /= 2.0;

  */

  /*

  // This estimate of the error uses the gradient of the
  // parabola as a measure of the goodness of fit. The
  // equation of the derivative is simply:
  //
  // Dy/Dx = 2Ax + B
  //
  // We use the slope of the parabola a distance one bin
  // away from the peak.

  double s1 = fabs(2.0 * A * (F-(1.0/get_nbin())));
  
  error = (1.0 / (s1*500.0));

  */

  // This estimate of the error is even simpler... Hopefully
  // it will overcome the limitations of the other two. The
  // slope-based method suffers from problems in absolute
  // offset... when the peak of the correlation function is
  // very high, the slope is artificially overestimated.
  // The width-based method doesn't scale properly because
  // it is locked too firmly to the bin width.
  //
  // The estimate defined here simply involves calculating when
  // the correlation function drops by a small fixed fraction
  // of its own peak value.

  float fracdrop = 1e-6;
  float height = D - (D * fracdrop);

  if ((D - height)/E < 0.0) {
    throw Error(FailedCall, "Profile::ParIntShift",
		"aborting before floating exception");
  }

  // The error in phase units
  float errwidth = (float(2.0 * sqrt((D - height)/E))) / 2.0;

  // The above doesn't work either, it is too insensitive to
  // poor parabolic fits. The next method estimates the error
  // by computing the standard deviation of the correlation
  // function points companed to the parabolic model.

  vector<float> parb;
  vector<float> real;
  vector<float> diff;

  for (int i = -3; i < 3; i++) {
    real.push_back(ptr->get_amps()[(maxbin + i)%get_nbin()]);
    // Don't wrap x!
    float x = float(maxbin + i)/float(get_nbin());
    parb.push_back(D - E*(x-F)*(x-F));
  }

  for (unsigned i = 0; i < parb.size(); i++) {
    diff.push_back(real[i] - parb[i]);
  }

  // We divide by 6.0 in an unjustified manner...
  float errfit = Pulsar::SampleStdDev(diff) / D;
  errfit /= 6.0;

  // The real error is assumed to be the combination of two effects:

  error = sqrt(errfit*errfit + errwidth*errwidth) / 2.0;

  // The shift in phase units, wrapped to be between -0.5 and 0.5

  if (F < -0.5)
    F += 1.0;
  else if (F > 0.5)
    F -= 1.0;

  return F;
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
