#include "PolnProfile.h"
#include "Profile.h"
#include "Error.h"

Reference::To<Pulsar::Profile> Pulsar::PolnProfile::null;

/*! When transforming Profile objects, the absolute gain of the
  transformation may artificially inflate the weight of the Profile
  and incorrectly skew mean results. */
bool Pulsar::PolnProfile::correct_weights = true;

Pulsar::PolnProfile::PolnProfile () :
  p0 (null), p1 (null), p2 (null), p3 (null)
{
  state = Signal::Stokes;
  basis = Signal::Linear;
}

Pulsar::PolnProfile::PolnProfile (Signal::Basis _basis, Signal::State _state, 
				  Reference::To<Profile>& _p0,
				  Reference::To<Profile>& _p1,
				  Reference::To<Profile>& _p2,
				  Reference::To<Profile>& _p3) :
  p0 (_p0), p1 (_p1), p2 (_p2), p3 (_p3)
{
  basis = _basis;
  state = _state;

  unsigned nbin = p0->get_nbin();
    
  if ( (p1->get_nbin() != nbin) ||
       (p2->get_nbin() != nbin) ||
       (p3->get_nbin() != nbin) )

    throw Error (InvalidParam, "Pulsar::PolnPofile::PolnProfile",
		 "unequal nbin");
}


Pulsar::PolnProfile::~PolnProfile ()
{

}

Stokes<float> Pulsar::PolnProfile::get_Stokes (unsigned ibin)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= p0->get_nbin())
    throw Error (InvalidRange, "PolnProfile::get_Stokes",
		 "ibin=%d >= nbin=%d", ibin, p0->get_nbin());

  return Stokes<float> (p0->get_amps()[ibin],
			p1->get_amps()[ibin],
			p2->get_amps()[ibin],
			p3->get_amps()[ibin]);
}

void Pulsar::PolnProfile::set_Stokes (unsigned ibin, 
				      const Stokes<float>& new_amps)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= p0->get_nbin())
    throw Error (InvalidRange, "PolnProfile::set_Stokes",
		 "ibin=%d >= nbin=%d", ibin, p0->get_nbin());

  p0->get_amps()[ibin] = new_amps.s0;
  p1->get_amps()[ibin] = new_amps.s1;
  p2->get_amps()[ibin] = new_amps.s2;
  p3->get_amps()[ibin] = new_amps.s3;

}


void Pulsar::PolnProfile::convert_state (Signal::State out_state)
{
  if (out_state == state)
    return;

  if (out_state == Signal::Stokes) {

    sum_difference (p0, p1);
    
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ]
    *(p2) *= 2.0;
    *(p3) *= 2.0;

    if (basis == Signal::Circular) {
      Reference::To<Profile> V = p1;
      Reference::To<Profile> Q = p2;
      Reference::To<Profile> U = p3;

      p1 = Q;
      p2 = U;
      p3 = V;
    }

    // record the new state
    state = Signal::Stokes;

  }
  else if (out_state == Signal::Coherence) {

    if (basis == Signal::Circular) {
      Reference::To<Profile> ReLR = p1;
      Reference::To<Profile> ImLR = p2;
      Reference::To<Profile> diffLR = p3;

      p1 = diffLR;
      p2 = ReLR;
      p3 = ImLR;
    }

    sum_difference (p0, p1);

    // The above sum and difference produced 2*PP and 2*QQ.  As well,
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ].
    *(p0) *= 0.5;
    *(p1) *= 0.5;
    *(p2) *= 0.5;
    *(p3) *= 0.5;

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
