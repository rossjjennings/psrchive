#include "Pulsar/Profile.h"
#include "Error.h"

/*!
  Calculates the shift between observed and template profiles.
  Returns a basic Tempo::toa object
*/

Tempo::toa Pulsar::Profile::toa (const Profile& std, const MJD& mjd,
				 double period, char nsite, string arguments,
				 Tempo::toa::Format fmt, bool td) const
{
  Tempo::toa retval (fmt);

  float  ephase, snrfft, esnrfft;
  double phase;
  
  if (td) {
    phase = ParIntShift(std, ephase);
  }
  else {
    phase = PhaseGradShift (std, ephase, snrfft, esnrfft);
  }

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
