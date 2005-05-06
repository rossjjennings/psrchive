#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Physical.h"
#include "Error.h"

void dispersive_phases (const Pulsar::Integration* integration,
		       vector<float>& phases)
{
  unsigned nchan = integration->get_nchan();
  phases.resize (nchan);

  if ( integration->get_dedispersed() ) {

    if (Pulsar::Integration::verbose)
      cerr << "dispersive_phases: data are dedispersed; no dispersive phases"
	   << endl;

    for (unsigned i=0; i<nchan; i++)
      phases[i] = 0.0;

    return;

  }

  double dm = integration->get_dispersion_measure();
  double pfold = integration->get_folding_period();
  double centrefreq = integration->get_centre_frequency();

  for (unsigned ichan=0; ichan<nchan; ichan++) {

    phases[ichan] = 0.0;

    if (pfold==0 || dm==0)
      return;

    double freq = integration->get_centre_frequency (ichan);

    if (Pulsar::Integration::verbose)
      cerr << "dispersive_phases: ichan=" << ichan << " nchan=" << nchan
           << " freq=" << freq << " cfreq=" << centrefreq << endl;

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
void Pulsar::Integration::remove_baseline (float phase, float dc)
try {

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::remove_baseline entered" << endl;

  if (phase == -1.0)
    phase = find_min_phase (dc);

  vector<float> phases;
  dispersive_phases (this, phases);

  for (unsigned ichan=0; ichan<get_nchan(); ichan++) {

    float chanphase = phase + phases[ichan];
	
    for (unsigned ipol=0; ipol<get_npol(); ipol++)
      *(profiles[ipol][ichan]) -= profiles[ipol][ichan]->mean (chanphase, dc);

  }
}
catch (Error& error) {
  throw error += "Integration::remove_baseline";
}

//! Return the statistics of every profile baseline
void
Pulsar::Integration::baseline_stats (vector<vector<Estimate<double> > >* mean,
				     vector< vector<double> >* variance) const
try {

  if (verbose)
    cerr << "Pulsar::Integration::baseline_stats" << endl;

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

      if (get_weight(ichan) == 0) {
        if (mean)
          (*mean)[ipol][ichan] = 0;
        if (variance)
          (*variance)[ipol][ichan] = 0;
        continue;
      }

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

  if (verbose)
    cerr << "Pulsar::Integration::baseline_stats exit" << endl;

}
catch (Error& error) {
  throw error += "Integration::baseline_stats";
}

