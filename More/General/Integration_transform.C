#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

void Pulsar::Integration::transform (const Jones<float>& response)
{
  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

    PolnProfile poln (get_basis(), get_state(), 
		      profiles[0][ichan], profiles[1][ichan],
		      profiles[2][ichan], profiles[3][ichan]);

    poln.transform (response);
    
  }
}
 
void Pulsar::Integration::transform (const vector< Jones<float> >& response)
{
  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  if (response.size() != get_nchan())
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "response size=%d != nchan=%d", response.size(), get_nchan());

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

    PolnProfile poln (get_basis(), get_state(), 
		      profiles[0][ichan], profiles[1][ichan],
		      profiles[2][ichan], profiles[3][ichan]);

    poln.transform (response[ichan]);
    
  }
}
 
