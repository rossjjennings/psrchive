#include "Integration.h"
#include "Profile.h"
#include "Physical.h"
#include "Error.h"

/*!
  If phase is not specified, this method calls
  Integration::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Integration::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the profiles over all polarizations and frequencies.  If the
  dispersion measure and folding period have been previously set, the
  baseline phase is shifted according to the dispersion relation.
  */
void Pulsar::Integration::remove_baseline (float phase)
{
  try {

    if (phase == -1.0)
      phase = find_min_phase ();

    double dm = get_dispersion_measure();
    double pfold = get_folding_period();
    double centrefreq = get_centre_frequency();

    for (int ichan=0; ichan<get_nchan(); ichan++) {

      float chanphase = phase;

      if (pfold && dm) {
	double chanfreq = profiles[0][ichan]->get_centre_frequency();
	double phase_off = dispersion_delay (dm, centrefreq, chanfreq) / pfold;
	chanphase += phase_off;
      }
	
      for (int ipol=0; ipol<get_npol(); ipol++)
	*(profiles[ipol][ichan]) -= profiles[ipol][ichan] -> mean (chanphase);

    }

  }
  catch (Error& error) {
    throw error += "Integration::remove_baseline";
  }
}
