#include "Pulsar/Profile.h"
#include "interpolate.h"

/* This algorithm uses zero padding in the fourier domain to
   interpolate the cross correlation function in the time domain. The
   technique is described at:
  
   http://www.dspguru.com/howto/tech/zeropad2.htm 
*/
Estimate<double>
Pulsar::ZeroPadShift (const Profile& std, const Profile& obs)
{
  // First compute the standard cross correlation function:

  Reference::To<Pulsar::Profile> ptr = obs.clone();
  Reference::To<Pulsar::Profile> stp = std.clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate(&std);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  vector< Estimate<float> > correlation;
  
  for (unsigned i = 0; i < obs.get_nbin(); i++) {
    correlation.push_back(ptr->get_amps()[i]);
  }
  
  vector< Estimate<float> > interpolated;
  
  interpolated.resize(correlation.size() * Pulsar::Profile::ZPSF);
  
  // Perform the zero-pad interpolation
  
  fft::interpolate(interpolated, correlation);
  
  // Find the peak of the correlation function
  
  float maxval = 0.0;
  float maxloc = 0.0;
  
  for (unsigned i = 0; i < interpolated.size(); i++) {
    if (interpolated[i].val > maxval) {
      maxval = interpolated[i].val;
      maxloc = float(i) / Pulsar::Profile::ZPSF;
    }
  }
  
  // Error estimate (???)
  float ephase = 1.0 / float(obs.get_nbin());
  
  double shift = double(maxloc) / double(obs.get_nbin());
  
  if (shift < -0.5)
    shift += 1.0;
  else if (shift > 0.5)
    shift -= 1.0;
  
  return Estimate<double>(shift,ephase*ephase);
}
