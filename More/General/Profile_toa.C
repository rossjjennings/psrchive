#include <math.h>

#include "Pulsar/Profile.h"
#include "Error.h"

#include "model_profile.h"
#include "GaussJordan.h"

/*! Calculates the shift relative to a standard profile using a time
  domain correlation to get course discrete estimates of the shift and
  a quadratic interpolation to find the point of maximum correlation.

  Returns a Tempo::toa object */

Tempo::toa Pulsar::Profile::tdt (const Profile& std, 
				 const MJD& mjd, 
				 double period, char nsite, string arguments,
				 Tempo::toa::Format fmt) const
{
  float error;

  double shift = TimeShift(std, error);
  
  Tempo::toa retval (fmt);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + shift * period);
  retval.set_error     (error * period * 1e3);

  retval.set_telescope (nsite);
  retval.set_auxilliary_text(arguments);

  return retval;
}

/*!
  Calculates the shift between
  Returns a basic Tempo::toa object
*/
Tempo::toa Pulsar::Profile::toa (const Profile& std, const MJD& mjd, 
				 double period, char nsite, string arguments,
				 Tempo::toa::Format fmt) const
{
  Tempo::toa retval (fmt);

  float ephase, snrfft, esnrfft; 
  double phase = shift (std, ephase, snrfft, esnrfft);

  retval.set_frequency (centrefreq);
  retval.set_arrival   (mjd + phase * period);
  retval.set_error     (ephase * period * 1e6);

  retval.set_telescope (nsite);
  retval.set_auxilliary_text(arguments);
  
  if (verbose) {
    fprintf (stderr, "Pulsar::Profile::toa created:\n");
    retval.unload (stderr);
  }
  return retval;
}

double Pulsar::Profile::TimeShift (const Profile& std, 
				   float& error) const
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

  // Phases can be outside the 0->1 range here, 
  // they get wrapped later

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
  // y = Ax^2 +Bx +C
  
  double A = matrix[0][0]*y1 + matrix[0][1]*y2 + matrix[0][2]*y3;
  double B = matrix[1][0]*y1 + matrix[1][1]*y2 + matrix[1][2]*y3;
  double C = matrix[2][0]*y1 + matrix[2][1]*y2 + matrix[2][2]*y3;

  // Now calculate the equivalent coefficients for the form:
  // y = D - E(x - F)^2
  //
  // Which has the physical interperatation:
  //   D => no use
  //   E => proportional to the TOA error
  //   F => the relative shift

  // Use the equations:
  //   A = -E       => E = -A
  //   B = 2EF      => F = -B/2A
  //   C = D - EF^2 => D = C - (B^2/4A)

  double D = C - (B*B/(4.0*A));
  double E = -1.0 * A;
  double F =  (-1.0 * B) / (2.0 * A);

  // The error estimate is somewhat speculative... It is
  // computed as the full width at half maximum of the
  // parabola, assuming the baseline is the average level
  // of the two bins either side the maximum bin in the 
  // correlation function.

  double height = y2 - (y1 + y3) / 2.0;
  height /= 2.0;

  if ((D - height)/E < 0.0) {
    throw Error(FailedCall, "Profile::TimeShift",
		"aborting before floating exception");
  }
  
  // The error in phase units
  error = float(2.0 * sqrt((D - height)/E));

  // The shift in phase units, wrapped to be between -1.0 and 1.0
  if (F < -1.0) {
    return (F + 1.0);
  }
  else if (F > 1.0) {
    return (F - 1.0);
  }
  else {
    return F;
  }
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
    ret = legacy_fftconv (nbin, &amps[0], &(std.amps[0]),&dshift,&sigma_dshift,&snrfft,&esnrfft);
  

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

