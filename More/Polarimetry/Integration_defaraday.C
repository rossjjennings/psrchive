#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Calibration/Faraday.h"

/*!
  \param rm rotation measure
  \param rm_iono rotation measure due to ionosphere

  \pre baseline has been removed
  \pre archive has full polarimetric information
*/
void Pulsar::Integration::defaraday (double rm)
{
  Calibration::Faraday faraday;

  faraday.set_rotation_measure( rm );
  faraday.set_reference_frequency( get_centre_frequency() );
  
  Reference::To<PolnProfile> poln_profile;

  Signal::State state = get_state();

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

    faraday.set_frequency( get_frequency(ichan) );

    poln_profile = new_PolnProfile (ichan);

    poln_profile->transform ( faraday.evaluate() );

    if (ichan == 0)
      state = poln_profile->get_state();

  }

  set_state (state);
 
}

