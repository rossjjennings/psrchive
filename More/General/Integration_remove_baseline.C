#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Physical.h"
#include "Error.h"

void dispersive_phases (const Pulsar::Integration* integration,
		       vector<float>& phases)
{
  double dm = integration->get_dispersion_measure();
  double pfold = integration->get_folding_period();
  double centrefreq = integration->get_centre_frequency();

  unsigned nchan = integration->get_nchan();

  phases.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    phases[ichan] = 0.0;

    if (pfold==0 || dm==0)
      return;

    double freq = integration->get_Profile(0, ichan)->get_centre_frequency();

    phases[ichan] = Pulsar::dispersion_delay (dm, centrefreq, freq) / pfold;
    
  }
}

/*!
  If phase is not specified, this method calls
  Integration::find_min_phase to find the phase at which the mean in a
  region of the total intensity (as returned by Integration::total)
  reaches a minimum.  This phase is then used to remove the baseline from
  each of the profiles over all polarizations and frequencies.  If the
  dispersion measure and folding period have been previously set, the
  baseline phase is shifted according to the dispersion relation.
  */
void Pulsar::Integration::remove_baseline (float phase)
{

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::remove_baseline entered" << endl;

  try {

    if (phase == -1.0)
      phase = find_min_phase ();

    vector<float> phases;
    dispersive_phases (this, phases);

    for (unsigned ichan=0; ichan<get_nchan(); ichan++) {

      float chanphase = phase + phases[ichan];
	
      for (unsigned ipol=0; ipol<get_npol(); ipol++)
	*(profiles[ipol][ichan]) -= profiles[ipol][ichan] -> mean (chanphase);

    }

  }
  catch (Error& error) {
    throw error += "Integration::remove_baseline";
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
void Pulsar::Integration::baseline_levels
(vector<vector<Estimate<double> > >& mean) const
{
  baseline_stats (&mean, 0);
}

//! Return the noise power in every profile baseline
void Pulsar::Integration::baseline_power
(vector< vector<double> >& variance) const
{
  baseline_stats (0, &variance);
}

//! Return the statistics of every profile baseline
void
Pulsar::Integration::baseline_stats (vector<vector<Estimate<double> > >* mean,
				     vector< vector<double> >* variance) const
{
  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::baseline_stats entered" << endl;

  try {

    float phase = find_min_phase ();

    vector<float> phases;
    dispersive_phases (this, phases);

    unsigned npol = get_npol();
    unsigned nchan = get_nchan();

    if (mean)
      mean->resize (npol);
    if (variance)
      variance->resize (npol);

    double* meanval_ptr = 0;
    double* meanvar_ptr = 0;
    double* variance_ptr = 0;

    for (unsigned ipol=0; ipol<npol; ++ipol) {

      if (mean)
	(*mean)[ipol].resize (nchan);
      if (variance)
	(*variance)[ipol].resize (nchan);

      for (unsigned ichan=0; ichan<nchan; ++ichan) {

	float chanphase = phase + phases[ichan];

	if (mean) {
	  meanval_ptr = &((*mean)[ipol][ichan].val);
	  meanvar_ptr = &((*mean)[ipol][ichan].var);
	}
	if (variance)
	  variance_ptr = &((*variance)[ipol][ichan]);

	profiles[ipol][ichan]->stats (chanphase, meanval_ptr,
				      variance_ptr, meanvar_ptr);

      }

    }

  }
  catch (Error& error) {
    throw error += "Integration::baseline_levels";
  }


}
