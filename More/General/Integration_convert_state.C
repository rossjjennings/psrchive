#include "Integration.h"
#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::convert_state
//
/*!

*/
void Pulsar::Integration::convert_state (Poln::State state)
{ try {

  if (get_poln_state() == state)
    return;

  //////////////////////////////////////////////////////////////////////////
  // convert from Coherence products to Stokes parameters
  if (get_poln_state() == Poln::Coherence && state == Poln::Stokes) {

    // take the sum and difference of PP and QQ
    intensity_mix (1, 2.0);

    if (get_feed_type() == Feed::Circular)
      // rotate Stokes VQU into QUV
      poln_cycle (1);

    set_poln_state ( state );
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  // convert from Stokes parameters to Coherence products
  if (get_poln_state() == Poln::Stokes && state == Poln::Coherence) {

    if (get_feed_type() == Feed::Circular)
      // rotate Stokes QUV into VQU
      poln_cycle (-1);

    // take the sum and difference of the total intensity and Q or V
    intensity_mix (1, 1.0);

    set_poln_state ( state );
    return;
  }

  if (state == Poln::Invariant) {
    // form the invariant interval
  }
  else if (state == Poln::Intensity) {
    // pscrunch
  }

}
catch (Error& error) {
  throw error += "Integration::convert_state";
}
  
 throw Error (InvalidPolnState, "Integration::convert_state",
	      "cannot convert from %s to %s", 
	      Poln::state_string (get_poln_state()),
	      Poln::state_string (state));
}

/*! 
  \param direction specifies the direction in which the indeces are cycled
  <UL>
  <LI> +1 : QUV -> UVQ
  <LI> -1 : QUV -> VQU
  </UL>
  This method simply shifts around pointers to achieve the desired effect.
*/
void Pulsar::Integration::poln_cycle (int direction)
{
  if (direction == 0)
    return;

  if (abs(direction) != 1)
    throw Error (InvalidParam, "Integration::poln_cycle", "bad direction");

  for (int ichan=0; ichan < get_nchan(); ichan++) {

    Profile* Q = profiles[1][ichan];
    Profile* U = profiles[2][ichan];
    Profile* V = profiles[3][ichan];

    if (direction == 1) {
      profiles[1][ichan] = U;
      profiles[2][ichan] = V;
      profiles[3][ichan] = Q;
    }
    else {
      profiles[1][ichan] = V;
      profiles[2][ichan] = Q;
      profiles[3][ichan] = U;
    }

  }
}

//! Mix the intensity with the specified polarization
void Pulsar::Integration::intensity_mix (int ipol, float fac)
{
  static Profile temp;

  for (int ichan=0; ichan < get_nchan(); ichan++) {

    // form the sum of the total intensity and the specified polarization
    temp = *(profiles[0][ichan]);
    temp += *(profiles[ipol][ichan]);
    temp *= fac;

    // form the difference
    *(profiles[ipol][ichan]) = *(profiles[0][ichan]);
    *(profiles[ipol][ichan]) -= *(profiles[ipol][ichan]);
    *(profiles[ipol][ichan]) *= fac;

    // store the sum
    *(profiles[0][ichan]) = temp;

  }
}
