#include "PolnProfile.h"
#include "Profile.h"
#include "Error.h"

void Pulsar::PolnProfile::init ()
{
  mine = false;
  state = Signal::Stokes;
  basis = Signal::Linear;
  data[0] = data[1] = data[2] = data[3] = 0;
}


Pulsar::PolnProfile::PolnProfile (Signal::Basis _basis, Signal::State _state, 
				  Profile* p0, Profile* p1,
				  Profile* p2, Profile* p3,
				  bool clone)
{
  basis = _basis;
  state = _state;

  unsigned nbin = p0->get_nbin();
    
  if ( (p1->get_nbin() != nbin) ||
       (p2->get_nbin() != nbin) ||
       (p3->get_nbin() != nbin) )
    throw Error (InvalidParam, "Pulsar::PolnPofile::PolnProfile",
		 "unequal nbin");

  if (clone) {
    data[0] = p0->clone();
    data[1] = p1->clone();
    data[2] = p2->clone();
    data[3] = p3->clone();
    mine = true;
  }
  else {
    data[0] = p0;
    data[1] = p1;
    data[2] = p2;
    data[3] = p3;
    mine = false;
  }
}

void Pulsar::PolnProfile::get_Profiles (Profile* &p0, Profile* &p1,
					Profile* &p2, Profile* &p3) const
{
  p0 = data[0];
  p1 = data[1];
  p2 = data[2];
  p3 = data[3];
}

Pulsar::PolnProfile::~PolnProfile ()
{
  if (mine)
    for (int i=0; i<4; i++)
      delete data[i];
}

Stokes<float> Pulsar::PolnProfile::get_Stokes (unsigned ibin)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= data[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::get_Stokes",
		 "ibin=%d >= nbin=%d", ibin, data[0]->get_nbin());

  return Stokes<float> (data[0]->get_amps()[ibin],
			data[1]->get_amps()[ibin],
			data[2]->get_amps()[ibin],
			data[3]->get_amps()[ibin]);
}

void Pulsar::PolnProfile::set_Stokes (unsigned ibin, 
				      const Stokes<float>& new_amps)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= data[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::set_Stokes",
		 "ibin=%d >= nbin=%d", ibin, data[0]->get_nbin());

  for (int i=0; i<4; i++)
    data[i]->get_amps()[ibin] = new_amps[i];
}


void Pulsar::PolnProfile::convert_state (Signal::State out_state)
{
  if (out_state == state)
    return;

  if (out_state == Signal::Stokes) {

    sum_difference (data[0], data[1]);
    
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ]
    *(data[2]) *= 2.0;
    *(data[3]) *= 2.0;

    if (basis == Signal::Circular) {
      Profile* V = data[1];
      Profile* Q = data[2];
      Profile* U = data[3];

      data[1] = Q;
      data[2] = U;
      data[3] = V;
    }

    // record the new state
    state = Signal::Stokes;

  }
  else if (out_state == Signal::Coherence) {

    if (basis == Signal::Circular) {
      Profile* ReLR = data[1];
      Profile* ImLR = data[2];
      Profile* diffLR = data[3];

      data[1] = diffLR;
      data[2] = ReLR;
      data[3] = ImLR;
    }

    sum_difference (data[0], data[1]);

    // The above sum and difference produced 2*PP and 2*QQ.  As well,
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ].
    for (unsigned ipol=0; ipol<4; ipol++)
      *(data[ipol]) *= 0.5;

    // record the new state
    state = Signal::Coherence;

  }
  else
    throw Error (InvalidParam, "Pulsar::PolnProfile::convert_state",
		 "invalid output state %s", Signal::state_string (out_state));
}

/*! \retval sum = sum + difference
    \retval difference = sum - difference
*/
void Pulsar::PolnProfile::sum_difference (Profile* sum, Profile* difference)
{
  unsigned nbin = sum->get_nbin();

  if (nbin != difference->get_nbin())
    throw Error (InvalidParam, "Pulsar::PolnProfile::sum_difference",
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
