#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::convert_state
//
/*!

*/
void Pulsar::Integration::convert_state (Signal::State state)
{ try {

  if (get_state() == state)
    return;

  //////////////////////////////////////////////////////////////////////////
  // convert to Coherence products or Stokes parameters
  if (state == Signal::Coherence || state == Signal::Stokes) {
    poln_convert (state);
    set_state ( state );
    return;
  }

  if (state == Signal::Invariant) {
    // form the invariant interval
  }
  else if (state == Signal::Intensity) {
    // pscrunch
  }

}
catch (Error& error) {
  throw error += "Pulsar::Integration::convert_state";
}
  
 throw Error (InvalidPolnState, "Pulsar::Integration::convert_state",
	      "cannot convert from %s to %s", 
	      Signal::state_string (get_state()),
	      Signal::state_string (state));
}


//! Converts between coherency products and Stokes parameters
/*! As it performs no error checking, this method should not be called 
  directly.  Use Pulsar::Integration::convert_state. */
void Pulsar::Integration::poln_convert (Signal::State out_state)
{
  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

    PolnProfile poln (get_basis(), get_state(), 
		      profiles[0][ichan], profiles[1][ichan],
		      profiles[2][ichan], profiles[3][ichan]);

    poln.convert_state (out_state);

    poln.get_Profiles (profiles[0][ichan], profiles[1][ichan],
		       profiles[2][ichan], profiles[3][ichan]);
  }
}
