#include "Pulsar/Integration.h"
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
  // convert from Coherence products to Stokes parameters
  if (get_state() == Signal::Coherence && state == Signal::Stokes) {

    poln_convert (state);

    if (get_basis() == Signal::Circular)
      // rotate Stokes VQU into QUV
      poln_cycle (1);

    set_state ( state );
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  // convert from Stokes parameters to Coherence products
  if (get_state() == Signal::Stokes && state == Signal::Coherence) {

    if (get_basis() == Signal::Circular)
      // rotate Stokes QUV into VQU
      poln_cycle (-1);

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

/*! As it performs no error checking, this method should not be called 
  directly.  Use Pulsar::Integration::convert_state.

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
    throw Error (InvalidParam, "Pulsar::Integration::poln_cycle",
		 "bad direction");

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

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

//! Converts between coherency products and Stokes parameters
/*! As it performs no error checking, this method should not be called 
  directly.  Use Pulsar::Integration::convert_state. */
void Pulsar::Integration::poln_convert (Signal::State out_state)
{
  if (out_state != Signal::Stokes && out_state != Signal::Coherence)
    throw Error (InvalidParam, "Pulsar::Integration::poln_convert",
		 "invalid output state %s", Signal::state_string (out_state));
    
  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {
    
    sum_difference (profiles[0][ichan], profiles[1][ichan]);
    
    if (out_state == Signal::Stokes) {
      // profiles 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ]
      *(profiles[2][ichan]) *= 2.0;
      *(profiles[3][ichan]) *= 2.0;
    }
    else if (out_state == Signal::Coherence) {
      // The above sum and difference produced 2*PP and 2*QQ.  As well,
      // profiles 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ].
      for (unsigned ipol=0; ipol<4; ipol++)
	*(profiles[ipol][ichan]) *= 0.5;
    }
  }
}

/*! \retval sum = sum + difference
    \retval difference = sum - difference
*/
void Pulsar::Integration::sum_difference (Profile* sum, Profile* difference)
{
  unsigned nbin = sum->get_nbin();

  if (nbin != difference->get_nbin())
    throw Error (InvalidParam, "Pulsar::Integration::sum_difference",
		 "nbin=%d != nbin=%d", nbin, difference->get_nbin());

  float* s = sum->get_amps ();
  float* d = difference->get_amps ();
  float temp;

  for (unsigned ibin=0; ibin<nbin; ibin++) {
    temp = s[ibin];
    s[ibin] += d[ibin];
    d[ibin] = temp - d[ibin];
  }
}
