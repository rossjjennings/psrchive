#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/*!
  \param standard
  \retval toas
*/
void Pulsar::Archive::toas (const Archive* standard, vector<Tempo::toa>& toas)
{
  Pulsar::Archive* stdarch = standard->clone();

  // Extract the standard profile
  stdarch->fscrunch();
  stdarch->tscrunch();
  stdarch->convert_state(Signal::Intensity);

  Pulsar::Profile* stdprof = stdarch->get_Profile(0,0,0);

  // Set up this archive
  fscrunch();
  convert_state(Signal::Intensity);

  Pulsar::Profile* prof;
  MJD mid_time;
  Phase phase;
  double period;

  for (unsigned i = 0; i < get_nsubint(); i++) {
    
    // Extract the profile from the Integration
    prof = get_Profile(i,0,0);

    // Extract the mid-time of the Integration
    mid_time = get_Integration(i)->get_mid_time();

    // Find the topocentric period and phase at the mid-time
    period = model.period(mid_time);
    phase = model.phase(mid_time, get_centre_frequency());
    
    // Calculate the adjustment required to refer the mid-time
    // to pulse phase zero
    double offset = phase.fracturns() * period;
    
    // Adjust the mid-time
    mid_time -= offset;
    
    toas.push_back(prof->toa(stdprof, mid_time, period,
			     get_telescope_code()));
  }

}
