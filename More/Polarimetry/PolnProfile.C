#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Pauli.h"
#include "Error.h"

#ifdef sun
#include <ieeefp.h>
#endif

/*! When transforming Profile objects, the absolute gain of the
  transformation may artificially inflate the weight of the Profile
  and incorrectly skew mean results. */
bool Pulsar::PolnProfile::correct_weights = true;

Pulsar::PolnProfile::PolnProfile ()
{
  state = Signal::Stokes;
  basis = Signal::Linear;
}

Pulsar::PolnProfile* Pulsar::PolnProfile::clone () const
{
  return new PolnProfile (basis, state,
			  profile[0]->clone(), profile[1]->clone(),
			  profile[2]->clone(), profile[3]->clone());
}

//
//
//
Pulsar::PolnProfile::PolnProfile (Signal::Basis _basis, Signal::State _state, 
				  Profile* _p0, Profile* _p1,
				  Profile* _p2, Profile* _p3)
{
  basis = _basis;
  state = _state;

  profile[0] = _p0;
  profile[1] = _p1;
  profile[2] = _p2;
  profile[3] = _p3;

  unsigned nbin = _p0->get_nbin();

  for (unsigned ipol=1; ipol < 4; ipol++)
    if (profile[ipol]->get_nbin() != nbin)
      throw Error (InvalidParam, "Pulsar::PolnPofile::PolnProfile",
		   "ipol=%d unequal nbin=%d != nbin=%d", ipol,
		   profile[ipol]->get_nbin(), nbin);
}

//
//
//
Pulsar::PolnProfile::~PolnProfile ()
{
  if (Profile::verbose)
    cerr << "Pulsar::PolnProfile destructor" << endl;
}

//
//
//
void Pulsar::PolnProfile::resize (unsigned nbin)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol]->resize (nbin);
}

//
//
//
unsigned Pulsar::PolnProfile::get_nbin () const
{
  return profile[0]->get_nbin();
}

//
//
//
const Pulsar::Profile* Pulsar::PolnProfile::get_Profile (unsigned ipol) const
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_Profile",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol];
}

Pulsar::Profile* Pulsar::PolnProfile::get_profile (unsigned ipol)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_Profile",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol];
}

//
//
//
const float* Pulsar::PolnProfile::get_amps (unsigned ipol) const
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_amps",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol]->get_amps();
}

//
//
//
float* Pulsar::PolnProfile::get_amps (unsigned ipol)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_amps",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol]->get_amps();
}

//
//
//
Stokes<float> Pulsar::PolnProfile::get_Stokes (unsigned ibin)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= profile[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::get_Stokes",
		 "ibin=%d >= nbin=%d", ibin, profile[0]->get_nbin());

  return Stokes<float> (profile[0]->get_amps()[ibin],
			profile[1]->get_amps()[ibin],
			profile[2]->get_amps()[ibin],
			profile[3]->get_amps()[ibin]);
}

//
//
//
void Pulsar::PolnProfile::set_Stokes (unsigned ibin, 
				      const Stokes<float>& new_amps)
{
  if (state != Signal::Stokes)
    convert_state (Signal::Stokes);

  if (ibin >= profile[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::set_Stokes",
		 "ibin=%d >= nbin=%d", ibin, profile[0]->get_nbin());

  profile[0]->get_amps()[ibin] = new_amps.s0;
  profile[1]->get_amps()[ibin] = new_amps.s1;
  profile[2]->get_amps()[ibin] = new_amps.s2;
  profile[3]->get_amps()[ibin] = new_amps.s3;

}

//
//
//
Jones<double> Pulsar::PolnProfile::get_coherence (unsigned ibin)
{
  if (state != Signal::Coherence)
    convert_state (Signal::Coherence);

  if (ibin >= profile[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::get_coherence",
                 "ibin=%d >= nbin=%d", ibin, profile[0]->get_nbin());

  complex<double> cross (profile[2]->get_amps()[ibin], 
                         profile[3]->get_amps()[ibin]);

  return Jones<double> (profile[0]->get_amps()[ibin], conj(cross),
                        cross, profile[1]->get_amps()[ibin]);
}

//
//
//
void Pulsar::PolnProfile::set_coherence (unsigned ibin,
                                         const Jones<double>& new_amps)
{
  if (state != Signal::Coherence)
    convert_state (Signal::Coherence);

  if (ibin >= profile[0]->get_nbin())
    throw Error (InvalidRange, "PolnProfile::set_coherence",
                 "ibin=%d >= nbin=%d", ibin, profile[0]->get_nbin());

  profile[0]->get_amps()[ibin] = new_amps.j(0,0).real();
  profile[1]->get_amps()[ibin] = new_amps.j(1,1).real();
  profile[2]->get_amps()[ibin] = new_amps.j(0,1).real();
  profile[3]->get_amps()[ibin] = new_amps.j(1,0).imag();

}

//
//
//
void Pulsar::PolnProfile::transform (const Jones<double>& response)
{
  if (Profile::verbose)
    cerr << "Pulsar::PolnProfile::transform response=" << response << endl;

  unsigned nbin = get_Profile(0)->get_nbin();

  float Gain = abs( det(response) );
  if (!finite(Gain))
    throw Error (InvalidParam, "Pulsar::PolnProfile::transform",
                 "non-invertbile response.  det(J)=%f", Gain);

  if (state == Signal::Stokes)
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      set_Stokes (ibin, ::transform (get_Stokes(ibin), response));
  
  else if (state == Signal::Coherence) {
    Jones<float> response_dagger = herm(response);
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      set_coherence (ibin, (response * get_coherence(ibin)) * response_dagger);
  }
  
  else
    throw Error (InvalidState, "Pulsar::PolnProfile::transform",
		 "unknown state=" + Signal::State2string(state));

  if (correct_weights)
    for (unsigned ipol=0; ipol < 4; ipol++)
      get_profile(ipol)->set_weight ( get_Profile(ipol)->get_weight() / Gain );

}

//
//
//
double Pulsar::PolnProfile::sum (int bin_start, int bin_end) const
{
  double sum = 0;
  for (unsigned ipol=0; ipol < 4; ipol++)
    sum += profile[ipol]->sum( bin_start, bin_end );
  return sum;
}

//
//
//
double Pulsar::PolnProfile::sumsq (int bin_start, int bin_end) const
{
  double sumsq = 0;
  for (unsigned ipol=0; ipol < 4; ipol++)
    sumsq += profile[ipol]->sumsq( bin_start, bin_end );
  return sumsq;
}

//
//
//
void Pulsar::PolnProfile::convert_state (Signal::State out_state)
{
  if (out_state == state)
    return;

  if (out_state == Signal::Stokes) {

    sum_difference (profile[0], profile[1]);
    
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ]
    *(profile[2]) *= 2.0;
    *(profile[3]) *= 2.0;

    if (basis == Signal::Circular) {
      float* V = profile[1]->get_amps();
      float* Q = profile[2]->get_amps();
      float* U = profile[3]->get_amps();

      profile[1]->amps = Q;
      profile[2]->amps = U;
      profile[3]->amps = V;
    }

    // record the new state
    state = Signal::Stokes;

  }
  else if (out_state == Signal::Coherence) {

    cerr << "convert_state to Signal::Coherence" << endl;

    if (basis == Signal::Circular) {
      float* ReLR   = profile[1]->get_amps();
      float* ImLR   = profile[2]->get_amps();
      float* diffLR = profile[3]->get_amps();

      profile[1]->amps = diffLR;
      profile[2]->amps = ReLR;
      profile[3]->amps = ImLR;
    }

    sum_difference (profile[0], profile[1]);

    // The above sum and difference produced 2*PP and 2*QQ.  As well,
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ].
    *(profile[0]) *= 0.5;
    *(profile[1]) *= 0.5;
    *(profile[2]) *= 0.5;
    *(profile[3]) *= 0.5;

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
