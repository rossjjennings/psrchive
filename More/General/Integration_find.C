#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::total
//
/*!
  This method is primarily designed for use by the Integration::find_*
  methods.  Integration::fscrunch is called with weighted_cfreq ==
  false so that the phase of the total profile will always relate to
  the centre frequency of the Integration.
*/
Pulsar::Integration* Pulsar::Integration::total () const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::total entered" << endl;

  if (get_npol()<1 || get_nchan()<1)
    throw Error (InvalidState, "Pulsar::Integration::total",
		 "npol=%d nchan=%d", get_npol(), get_nchan());

  int npol_keep = 1;
  if (get_state() == Signal::Coherence || get_state() == Signal::PPQQ)
    npol_keep = 2;

  Integration* copy = 0;

  try {
    copy = clone (npol_keep);
    copy->fscrunch (0, false);
    copy->pscrunch ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Integration::total";
  }
  
  return copy;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_transitions
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_transitions on the total intensity
  </UL>
*/
void Pulsar::Integration::find_transitions (int& hi2lo, int& lo2hi, int& buf)
  const
{
  Integration* copy = 0;

  try {
    copy = total ();
    copy->profiles[0][0]->find_transitions (hi2lo, lo2hi, buf);
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Integration::find_transitions";
  }
  delete copy;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_peak_edges
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_peak_edges on the total intensity
  </UL>
*/
void Pulsar::Integration::find_peak_edges (int& rise, int& fall) const
{
  Integration* copy = 0;

  try {
    copy = total ();
    copy->profiles[0][0]->find_peak_edges (rise, fall);
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Integration::find_peak_edges";
  }
  delete copy;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_min_phase
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_min_phase on the total intensity
  </UL>
*/
float Pulsar::Integration::find_min_phase () const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::find_min_phase entered" << endl;

  Integration* copy = 0;
  float min_phase = -1.0;

  try {
    copy = total ();
    min_phase = copy->profiles[0][0]->find_min_phase ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Integration::find_peak_edges";
  }
  delete copy;

  return min_phase;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_max_phase
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_max_phase on the total intensity
  </UL>
*/
float Pulsar::Integration::find_max_phase () const
{
  Integration* copy = 0;
  float max_phase = -1.0;

  try {
    copy = total ();
    max_phase = copy->profiles[0][0]->find_max_phase ();
  }
  catch (Error& err) {
    if (copy) delete copy;
    throw err += "Integration::find_peak_edges";
  }
  delete copy;

  return max_phase;
}

#if 0
void Pulsar::Integration::cal_levels (vector<Stokes>& hi,
			  vector<Stokes>& lo)
{
  if (nbin<1)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "nbin<1");
  if (npol!=4)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "npol!=4");
  if (nsub<1)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "nsub<1");

  hi.resize(nsub);
  lo.resize(nsub);

  int hightolow, lowtohigh, buffer;
  find_cal_transitions (hightolow, lowtohigh, buffer);

  Stokes zero;
  for (int ichan=0; ichan<nsub; ++ichan) {
    if (wts[ichan]==0) {
      hi[ichan] = lo[ichan] = zero;
      continue;
    }
    for (int ipol=0; ipol<4; ++ipol) {
      profiles[ipol][ichan]->stats (&hi[ichan][ipol], 0, 0,
				    lowtohigh + buffer,
				    hightolow - buffer);
      profiles[ipol][ichan]->stats (&lo[ichan][ipol], 0, 0,
				    hightolow + buffer,
				    lowtohigh - buffer);
    }
  }
}

void Pulsar::Integration::psr_levels (vector<Stokes>& hi,
			  vector<Stokes>& lo, float window)
{
  if (nbin<1)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "nbin<1");
  if (npol!=4)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "npol!=4");
  if (nsub<1)
    throw Error (InvalidState, "Pulsar::Integration::cal_levels",
		 "nsub<1");

  hi.resize(nsub);
  lo.resize(nsub);

  int ichan=0, ipol=0;

  // first, find the power in each pulse profile;
  Pulsar::Integration copy (*this);
  copy.remove_baseline(0, window);
  for (ichan=0; ichan<nsub; ++ichan)
    for (ipol=0; ipol<4; ++ipol)
      hi[ichan][ipol] = profiles[ipol][ichan]->sum()/nbin;

  // then, find the phase centre of the baseline in Stokes I
  copy.fscrunch ();
  // if only XX, add YY
  if (copy.profiles[0][0]->poln == 1)
    *(copy.profiles[0][0]) += *(copy.profiles[1][0]);

  float phase;
  copy.profiles[0][0]->min (&phase, window);

  Stokes zero;
  for (ichan=0; ichan<nsub; ++ichan) {

    if (wts[ichan]==0) {
      hi[ichan] = lo[ichan] = zero;
      continue;
    }

    // shift the phase centre of the baseline for each freq channel
    double fcentre = profiles[0][ichan]->centrefreq;
    double shift = dm/(2.41e-4)*(1.0/(fcentre*fcentre)-
				 1.0/(centrefreq*centrefreq)) / mini.pfold;

    int min = (phase + shift + window/2) * nbin;
    int max = (phase + shift - window/2) * nbin;

    for (ipol=0; ipol<4; ++ipol)
      profiles[ipol][ichan]->stats (&lo[ichan][ipol], 0, 0, min, max);

  }
}


// /////////////////////////////////////////////////////////////////////////
// baseline_levels
//
// for each profile
//   pulse = profile with baseline removed
//   noise = profile minus pulse
//   find mean and var_mean of noise
// 
void Pulsar::Integration::baseline_levels (vector<vector<double> > & mean,
			       vector<vector<double> > & var_mean,
			       float duty_cycle) const
{
  if (nbin==0)
    throw Error (InvalidState, "Pulsar::Integration::baseline_levels",
		 "nbin = 0");

  if (npol==0)
    throw Error (InvalidState, "Pulsar::Integration::baseline_levels",
		 "npol = 0");

  if (nsub==0)
    throw Error (InvalidState, "Pulsar::Integration::baseline_levels",
		 "nsub = 0");

  mean.resize(npol);
  var_mean.resize(npol);

  for (int ipol=0; ipol<npol; ++ipol) {

    mean[ipol].resize(nsub);
    var_mean[ipol].resize(nsub);

    for(int ichan=0; ichan<nsub; ++ichan) {

      if (wts[ichan]==0) {
	mean[ipol][ichan] = var_mean[ipol][ichan] = 0.0;
	continue;
      }
      profiles[ipol][ichan]->baseline_levels (&mean[ipol][ichan],
					      &var_mean[ipol][ichan],
					      duty_cycle);
    }  // for each frequency
  }  // for each polarization
}

#endif
