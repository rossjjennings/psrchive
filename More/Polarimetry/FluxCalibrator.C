#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

template <typename T> T sqr (T x) { return x*x; }
template <typename T> T fourth_power (T x) { return sqr(x)*sqr(x); }

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

  unsigned npol = archive->get_npol ();
  unsigned nchan = archive->get_nchan ();
  
  cal_flux.resize (npol);
  T_sys.resize (npol);
    
  vector<vector<MeanEstimate<double> > > mean_ratio_on (npol);
  vector<vector<MeanEstimate<double> > > mean_ratio_off (npol);

  for (unsigned ipol=0; ipol<npol; ++ipol) {
    cal_flux[ipol].resize(nchan);
    T_sys[ipol].resize(nchan);
    mean_ratio_on.resize(nchan);
    mean_ratio_off.resize(nchan);
  }
  for (iarch=0; iarch<archs.size(); ++iarch) {

    if (verbose) 
      cerr << "Pulsar::FluxCalibrator call Pulsar::Integration::cal_levels "
	   << archs[iarch]->get_filename() << endl;

    vector<vector<Estimate<double> > > cal_hi;
    vector<vector<Estimate<double> > > cal_lo;
  
    archs[iarch]->get_Integration(0)->cal_levels (cal_hi, cal_lo);

    for (unsigned ipol=0; ipol<npol; ++ipol) {
      
      for (unsigned ichan=0; ichan<nchan; ++ichan) {

	Estimate<double> ratio = cal_hi[ipol][ichan]/cal_lo[ipol][ichan] - 1.0;

	if (archs[iarch]->get_type() == Signal::FluxCalOn)
	  mean_ratio_on[ipol][ichan] += ratio;
	else if (archs[iarch]->get_type() == Signal::FluxCalOff)
	  mean_ratio_off[ipol][ichan] += ratio;

      }
    }
  }

  double hyd_mJy = hydra_flux_mJy (archive->get_centre_frequency());

  for (unsigned ichan=0; ichan<nchan; ++ichan) {
    for (unsigned ipol=0; ipol<npol; ++ipol) {

      if (mean_ratio_on[ipol][ichan]==0 || mean_ratio_off[ipol][ichan]==0) {
	for (unsigned lpol=0; lpol<npol; ++lpol)
	  cal_flux[lpol][ichan] = T_sys[lpol][ichan] = 0;
	break;
      }

      Estimate<double> on = mean_ratio_on[ipol][ichan].get_Estimate();

      Estimate<double> off = mean_ratio_off[ipol][ichan].get_Estimate();

      Estimate<double> r_diff = 1.0/on - 1.0/off;

      cal_flux[ipol][ichan] = hyd_mJy/r_diff;

      T_sys[ipol][ichan] = cal_flux[ipol][ichan]/off;

      if (cal_flux[ipol][ichan].val < sqrt(cal_flux[ipol][ichan].var)
	  || T_sys[ipol][ichan].val < sqrt(T_sys[ipol][ichan].var) ) {

	if (verbose)
	  cerr << "Pulsar::FluxCalibrator"
	    " channel=" << ichan  << " poln=" << ipol << ": low signal\n"
	       << "\t\tratio on " << on << " ratio off" << off << endl
	       << "\t\tcal flux " << cal_flux[ipol][ichan].val
	       << " cal error " << cal_flux[ipol][ichan].var << endl
	       << "\t\tsys flux " << T_sys[ipol][ichan].val
	       << " sys error " << T_sys[ipol][ichan].var << endl;

	for (unsigned l=0; l<npol; ++l)
	  cal_flux[l][ichan] = T_sys[l][ichan] = 0;

	break;
      }
      
    }
    
  }
  
}

