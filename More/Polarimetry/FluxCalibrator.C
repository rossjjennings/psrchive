#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
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
  
  archive = 0;

  unsigned iarch=0;
  for (; iarch<archs.size(); ++iarch) {

    if ( archs[iarch]->get_type() != Signal::FluxCalOn &&
	 archs[iarch]->get_type() != Signal::FluxCalOn )

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

    archnames.push_back (archs[iarch]->get_filename());
  }

  if (!archive)
    throw Error (InvalidState, "Pulsar::FluxCalibrator",
		 "No FluxCal-On Observation Provided");

  unsigned npol = archive->get_npol ();
  unsigned nchan = archive->get_nchan ();
  
  cal_flux.resize (npol);
  T_sys.resize (npol);
  
  vector<vector<Estimate<double> > > ratio_hi (npol);
  vector<vector<Estimate<double> > > ratio_lo (npol);

  for (int ipol=0; ipol<npol; ++ipol) {
    cal_flux[ipol].resize(nchan);
    Ts_sys[ipol].resize(nchan);
    ratio_hi.resize(nchan);
    ratio_lo.resize(nchan);
  }
  
  double hyd_mJy = hydra_flux_mJy (archive->get_centre_frequency());
  
  for (iarch=0; iarch<archs.size(); ++iarch) {

    if (verbose) 
      cerr << "Pulsar::FluxCalibrator call Pulsar::Integration::cal_levels "
	   << archs[iarch]->get_filename() << endl;

    vector<vector<Estimate<double> > > hi;
    vector<vector<Estimate<double> > > lo;
  
    archs[iarch]->get_Integration(0)->cal_levels (hi, lo);

    for (unsigned ipol=0; ipol<npol; ++ipol) {
      
      for (unsigned ichan=0; ichan<nchan; ++ichan) {

	double hi = hi[ipol][ichan].val;
	double var_hi = hi[ipol][ichan].var;
	double lo = lo[ipol][ichan].val;
	double var_lo = varlo[ipol][ichan].var;

	if (lo == 0){         // Bad channel
	  if (verbose) cerr << "Pulsar::FluxCalibrator::create"
			 " - levels flagged by find_cal_levels for channel " 
			    << ichan << " probe " << ipol
			    << " archive " << archs[i]->get_filename() << endl;
	  continue;  
	}

	double r = hi/lo;
	double ratio = r - 1.0;

	if (ratio<0) {
	  if (verbose) cerr << "Pulsar::FluxCalibrator::create"
			 " - ratio < 0 for channel " << ichan << " probe " << ipol
			    << " archive " << archs[i]->get_filename() << "\t" 
			    << hi  << "\t" << lo << endl;
	  continue;
	}

	// WvS : some basic stuff on errors:
	// Where:
	//    y = f (x1, x2, ... xn)
	// Then:
	// V(y) = d(f,x1)^2 * V(x1) + ... + d(f,xn)^2 * V(xn)
	// Where: V(x)   is variance of x
	//        d(f,x) is partial derivative of f wrt x

	double sigsq_ratio = r*r * ( var_hi/(hi*hi) + var_lo/(lo*lo) );

	if (archs[i]->get_type() == Signal::FluxCalOn) {
	  ratio_hi[ipol][ichan].val += ratio/sigsq_ratio;
	  ratio_hi[ipol][ichan].var += 1/sigsq_ratio;
	} else if (archs[i]->get_type() == Signal::FluxCalOff) {
	  ratio_lo[ipol][ichan].val += ratio/sigsq_ratio;
	  ratio_lo[ipol][ichan].var += 1/sigsq_ratio;
	}
      }
    }
  }

  for (unsigned ichan=0; ichan<nchan; ++ichan) {
    for (unsigned ipol=0; ipol<npol; ++ipol) {
      if (ratio_hi[ipol][ichan].val==0 || ratio_lo[ipol][ichan].val==0) {

	for (int lpol=0; lpol<npol; ++lpol)
	  cal_flux[lpol][ichan] = T_sys[lpol][ichan] = 0;

	break;
      }

      double hi_var = 1.0/ratio_hi[ipol][ichan].var;
      double hi_val *= ratio_hi[ipol][ichan].val * hi_var;

      double lo_var = 1.0/ratio_lo[ipol][ichan].var;
      double lo_val = ratio_lo[ipol][ichan].val * l_var;

      double r_diff = 1/hi_val - 1/lo_val;
      double r_on_4th = fourth_power (hi_val);
      double r_off_4th = fourth_power (lo_val);
      double r_diff_4th = fourth_power (r_diff);

      cal_flux[ipol][ichan].val = hyd_mJy/r_diff;
      cal_flux[ipol][ichan].var = (hi_var/r_on_4th + lo_var/r_off_4th)
	* hyd_mJy * hyd_mJy / r_diff_4th;

      T_sys[ipol][ichan].val = cal_flux[ipol][ichan].val/lo_val;
      T_sys[ipol][ichan].var = cal_flux[ipol][ichan].var/(lo_val*lo_val) + 
	  cal_flux[ipol][ichan]*cal_flux[ipol][ichan]*lo_var/r_off_4th;
	
      if (cal_flux[ipol][ichan] < sqrt(sigsq_cal_flux[ipol][ichan])
	  || Ts[ipol][ichan] < sqrt(sigsq_Ts[ipol][ichan]) ) {

	if (verbose) cerr << "Pulsar::FluxCalibrator::create - channel " << ichan 
			  << " poln " << ipol 
			  << ": flagging channel with low signal" << endl;
	if (verbose) cerr << "\t\tratio on " << hi_val 
			  << " ratio off n " << lo_val_north 
			  << " ratio off s " << lo_val_south << endl;
	if (verbose) cerr << "\t\tcal flux " << cal_flux[ipol][ichan]
			  << " cal error " << sigsq_cal_flux[ipol][ichan] << endl;
	if (verbose) cerr << "\t\tsys flux " << Ts[ipol][ichan] 
			  << " sys error " << sigsq_Ts[ipol][ichan] << endl << endl;
	for (int l=0; l<npol; ++l)
	  cal_flux[l][ichan]=sigsq_cal_flux[l][ichan] = Ts[l][ichan]=sigsq_Ts[l][ichan] = 0;
	break;
      }

    }

  }

}
#endif
