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
bool Pulsar::PolnProfile::normalize_weight_by_absolute_gain = false;

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
Pulsar::PolnProfile::PolnProfile (unsigned nbin)
{
  state = Signal::Stokes;
  basis = Signal::Linear;

  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol] = new Profile (nbin);
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
  for (unsigned ipol=0; ipol < 4; ipol++) {
    if (!profile[ipol])
      profile[ipol] = new Profile;
    profile[ipol]->resize (nbin);
  }
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
Pulsar::Profile* Pulsar::PolnProfile::get_Profile (unsigned ipol)
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
void Pulsar::PolnProfile::set_amps (unsigned ipol, float* amps)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::set_amps",
                 "ipol=%d >= npol=4", ipol);

  profile[ipol]->set_amps(amps);
}

//
//
//
Stokes<float> Pulsar::PolnProfile::get_Stokes (unsigned ibin) const
{
  if (state != Signal::Stokes)
    const_cast<PolnProfile*>(this)->convert_state (Signal::Stokes);

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

  profile[0]->get_amps()[ibin] = new_amps[0];
  profile[1]->get_amps()[ibin] = new_amps[1];
  profile[2]->get_amps()[ibin] = new_amps[2];
  profile[3]->get_amps()[ibin] = new_amps[3];
}

//
//
//
Jones<double> Pulsar::PolnProfile::get_coherence (unsigned ibin) const
{
  if (state != Signal::Coherence)
    const_cast<PolnProfile*>(this)->convert_state (Signal::Coherence);

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

  profile[0]->get_amps()[ibin] = new_amps(0,0).real();
  profile[1]->get_amps()[ibin] = new_amps(1,1).real();
  profile[2]->get_amps()[ibin] = new_amps(0,1).real();
  profile[3]->get_amps()[ibin] = new_amps(1,0).imag();

}

void Pulsar::PolnProfile::scale (double scale)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol]->scale (scale);
}

void Pulsar::PolnProfile::rotate_phase (double phase)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol]->rotate_phase (phase);
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

  if (Gain == 0) {

    if (Profile::verbose)
      cerr << "Pulsar::PolnProfile::transform zero response" << endl;

    for (unsigned ipol=0; ipol < 4; ipol++)
      get_profile(ipol)->set_weight ( 0.0 );

    return;
  }

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

  if (normalize_weight_by_absolute_gain)
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

    // cerr << "convert_state to Signal::Coherence" << endl;

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
void Pulsar::PolnProfile::convert_basis (Signal::Basis to) {
// The way this works is simple. If an archive has been formaed assuming a linear basis
// and the true basis was actually circular - then the Stokes parameters in a full Stokes archvie will be misslabeled
// I think ...
//
// linear basis
// ============
// S = |L|^2 + |R|^2
// Q = |L|^2 - |R|^2
// U = 2Re(L*R)
// V = 2Im(L*R)
//
// Circular basis
// ==============
// S = |L|^2 + |R|^2
// Q = 2Re(L*R)
// U = 2Im(L*R)
// V = |L|^2 - |R|^2
//
// Which means to go from one to the other - Say in the example that you somehow decided to produce Stokes 
// assuming you had linear feeds but it later turns out that they were circular - well you have to
// S->S
// Q->V
// U->Q
// V->U
// Then tyou make sure the basis label is Signal::Circular and robert's your mum's brother.
//
// 

  if (state == Signal::Stokes) {
    if (to == Signal::Circular) {
      cout << "Converting to Circular" << endl;
      float* V = profile[1]->get_amps();
      float* Q = profile[2]->get_amps();
      float* U = profile[3]->get_amps();

      profile[1]->amps = Q;
      profile[2]->amps = U;
      profile[3]->amps = V;
    }  
  } 
}
				      

/*! 
  Forms the Stokes polarimetric invariant interval,
  \f$\sqrt{I^2-Q^2-U^2-V^2}\f$, for every phase bin so that,
  upon completion, npol == 1 and state == Signal::Invariant.

  \pre The profile baselines must have been removed (unchecked).
*/
void Pulsar::PolnProfile::invint (Profile* invint) const
{
  unsigned nbin = get_nbin();

  invint->resize (nbin);

  if (state == Signal::Stokes)
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      invint->get_amps()[ibin] = get_Stokes(ibin).invariant ();
  
  else if (state == Signal::Coherence)
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      invint->get_amps()[ibin] = 4.0 * abs( det(get_coherence(ibin)) );

  // remove the baseline
  *(invint) -= invint->mean( invint->find_min_phase() );
  // return to a second-order moment
  invint->square_root();

  invint->set_state (Signal::Inv);
  invint->set_centre_frequency ( get_Profile(0)->get_centre_frequency() );
  invint->set_weight ( get_Profile(0)->get_centre_frequency() );
}

/*!

  \pre The PolnProfile must contain Stokes parameters; so as to reduce the
  amount of behind the scenes cloning and state conversion

  \pre The Profile baselines should have been removed; so as not to
  interfere with any baseline removal algorithms already applied

  \post The polarized Profile baseline will not have been removed; so as to
  enable the application of specialized baseline removal algorithms

 */
void Pulsar::PolnProfile::get_polarized (Profile* polarized) const
{
  if (state != Signal::Stokes)
    throw Error (InvalidState, "Pulsar::PolnProfile::get_PA",
		 "must first convert to Stokes parameters");

  const float *q = get_Profile(1)->get_amps();
  const float *u = get_Profile(2)->get_amps(); 
  const float *v = get_Profile(3)->get_amps();

  unsigned ibin, nbin = get_nbin();
  
  polarized->resize (nbin);
  float* amps = polarized->get_amps();

  for (ibin=0; ibin<nbin; ibin++)
    amps[ibin] = q[ibin]*q[ibin] + u[ibin]*u[ibin] + v[ibin]*v[ibin];

  float min_phase = polarized->find_min_phase();
  polarized->offset(-polarized->mean (min_phase));

  for (ibin=0; ibin<nbin; ibin++)
    if (amps[ibin] < 0.0)
      amps[ibin] = 0.0;
    else
      amps[ibin] = sqrt(amps[ibin]);
}


/*!

  \pre The PolnProfile must contain Stokes parameters; so as to reduce the
  amount of behind the scenes cloning and state conversion

  \pre The Profile baselines should have been removed; so as not to
  interfere with any baseline removal algorithms already applied

  \post The bias due to noise in Q and U will have been removed

 */
void Pulsar::PolnProfile::get_linear (Profile* linear) const
{
  if (state != Signal::Stokes)
    throw Error (InvalidState, "Pulsar::PolnProfile::get_PA",
		 "must first convert to Stokes parameters");

  const float *q = get_Profile(1)->get_amps();
  const float *u = get_Profile(2)->get_amps(); 

  unsigned ibin, nbin = get_nbin();
  
  linear->resize (nbin);
  float* amps = linear->get_amps();

  for (ibin=0; ibin<nbin; ibin++)
    amps[ibin] = q[ibin]*q[ibin] + u[ibin]*u[ibin];

  float min_phase = linear->find_min_phase();
  linear->offset(-linear->mean (min_phase));

  for (ibin=0; ibin<nbin; ibin++)
    if (amps[ibin] < 0.0)
      amps[ibin] = 0.0;
    else
      amps[ibin] = sqrt(amps[ibin]);
}

void Pulsar::PolnProfile::get_PA (vector< Estimate<double> >& posang,
				  float threshold) const
{
  if (state != Signal::Stokes)
    throw Error (InvalidState, "Pulsar::PolnProfile::get_PA",
		 "must first convert to Stokes parameters");
   
  Profile linear;
  get_linear (&linear);
  float min_phase = linear.find_min_phase();

  double mean = 0;
  double var_Q = 0;
  get_Profile(1)->stats (min_phase, &mean, &var_Q);
  if (abs(mean) > sqrt(var_Q))
    cerr << "Pulsar::PolnProfile::get_PA WARNING off-pulse Q mean="
	 << mean << " > rms=" << sqrt(var_Q) << endl;

  double var_U = 0;
  get_Profile(2)->stats (min_phase, &mean, &var_U);
  if (abs(mean) > sqrt(var_U))
    cerr << "Pulsar::PolnProfile::get_PA WARNING off-pulse U mean="
	 << mean << " > rms=" << sqrt(var_U) << endl;

  float sigma = sqrt (0.5*(var_Q + var_U));

  unsigned nbin = get_nbin();

  const float *q = get_Profile(1)->get_amps();
  const float *u = get_Profile(2)->get_amps(); 

  posang.resize (nbin);

  cerr << "var_Q=" << var_Q << " var_U=" << var_U << endl;

  for (unsigned ibin=0; ibin<nbin; ibin++) {
    if (!threshold || linear.get_amps()[ibin] > threshold*sigma) {
      Estimate<double> U (u[ibin], var_U);
      Estimate<double> Q (q[ibin], var_Q);

      posang[ibin] = 90.0/M_PI * atan2 (U, Q);
    }
    else
      posang[ibin] = 0.0;
  }

}
