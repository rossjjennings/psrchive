#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

bool Pulsar::FluxCalibrator::verbose = false;

//! Given the observing frequency in MHz, returns the flux of Hydra in mJy
double Pulsar::FluxCalibrator::hydra_flux_mJy (double cfreq)
{
  return pow (cfreq/1400.0, -.91) * 43.1 * 1000;
}


Pulsar::FluxCalibrator::FluxCalibrator (const vector<Pulsar::Archive*>& archs)
{
  if (archs.size()==0)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator",
		 "error empty Pulsar::Archive vector");
  
  unsigned iarch=0;
  for (iarch=0; iarch<archs.size(); ++iarch) {

    if ( archs[iarch]->get_type() != Signal::FluxCalOn &&
	 archs[iarch]->get_type() != Signal::FluxCalOff )

      throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		   + archs[iarch]->get_filename() + "' not a FluxCal");

    string reason;
    if (iarch > 0 && !archs[0]->mixable (archs[iarch], reason))
      throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		   + archs[iarch]->get_filename() + "'\ndoes not mix with '"
		   + archs[0]->get_filename() + "\n" + reason);

    if (!archive && archs[iarch]->get_type() == Signal::FluxCalOn)
      // Keep the FPTM naming convention in which the
      // Pulsar::FluxCalibrator is named for the first on-source
      // observation
      archive = archs[iarch];

    filenames.push_back (archs[iarch]->get_filename());
  }

  if (!archive)
    throw Error (InvalidState, "Pulsar::FluxCalibrator",
		 "No FluxCal-On Observation Provided");

  unsigned nchan = archive->get_nchan ();
  
  cal_flux.resize (nchan);
  T_sys.resize (nchan);
    
  vector<MeanEstimate<double> > mean_ratio_on (nchan);
  vector<MeanEstimate<double> > mean_ratio_off (nchan);

  for (iarch=0; iarch<archs.size(); ++iarch) {

    if (verbose) 
      cerr << "Pulsar::FluxCalibrator call Pulsar::Integration::cal_levels "
	   << archs[iarch]->get_filename() << endl;

    const Pulsar::Integration* integration;

    if (archs[iarch]->get_state () == Signal::Intensity)
      integration = archs[iarch]->get_Integration(0);
    else {
      Pulsar::Integration* clone = archs[iarch]->get_Integration(0)->clone();
      clone->convert_state (Signal::Intensity);
      integration = clone;
    }

    vector<vector<Estimate<double> > > cal_hi;
    vector<vector<Estimate<double> > > cal_lo;

    integration->cal_levels (cal_hi, cal_lo);

    for (unsigned ichan=0; ichan<nchan; ++ichan) {

      // Take the ratio of the total intensity
      Estimate<double> ratio = cal_hi[0][ichan]/cal_lo[0][ichan] - 1.0;
      
      if (archs[iarch]->get_type() == Signal::FluxCalOn)
	mean_ratio_on[ichan] += ratio;
      else if (archs[iarch]->get_type() == Signal::FluxCalOff)
	mean_ratio_off[ichan] += ratio;
      
    }
  }

  double hyd_mJy = hydra_flux_mJy (archive->get_centre_frequency());

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (mean_ratio_on[ichan]==0 || mean_ratio_off[ichan]==0) {
      cal_flux[ichan] = T_sys[ichan] = 0;
      continue;
    }

    Estimate<double> ratio_on = mean_ratio_on[ichan].get_Estimate();
    Estimate<double> ratio_off = mean_ratio_off[ichan].get_Estimate();

    Estimate<double> ratio_diff = 1.0/ratio_on - 1.0/ratio_off;

    cal_flux[ichan] = hyd_mJy/ratio_diff;

    T_sys[ichan] = cal_flux[ichan]/ratio_off;

    if (cal_flux[ichan].val < sqrt(cal_flux[ichan].var)
	|| T_sys[ichan].val < sqrt(T_sys[ichan].var) ) {
      
      if (verbose)
	cerr << "Pulsar::FluxCalibrator channel=" << ichan  << ": low signal"
	  "\n\t\tratio on=" << ratio_on << " ratio off=" << ratio_off <<
	  "\n\t\tcal flux=" << cal_flux[ichan] <<
	  "\n\t\tsys flux=" << T_sys[ichan] << endl;
      
      cal_flux[ichan] = T_sys[ichan] = 0;
    }
    
  }
  
}

