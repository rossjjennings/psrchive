#include "Integration.h"
#include "Profile.h"
#include "Physical.h"
#include "Error.h"

/*!

  If phase is not specified, this method calls
  Integration::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Integration::total) 
  reaches a minimum
  */
void Pulsar::Integration::remove_baseline (float phase)
{
  try {

    if (phase == -1.0)
      phase = find_min_phase ();

    for (int ichan=0; ichan<nchan; ichan++) {

      float chanphase = phase;

      if (pfold && dm) {
	double chanfreq = profiles[0][ichan]->get_centre_frequency();
	double phase_off = dispersion_delay (dm, centrefreq, chanfreq) / pfold;
	chanphase += phase_off;
      }
	
      for (int ipol=0; ipol<npol; ipol++)
	*(profiles[ipol][ichan]) -= profiles[ipol][ichan] -> mean (chanphase);

    }

  }
  catch (Error& error) {
    throw error += "Integration::remove_baseline";
  }
}
