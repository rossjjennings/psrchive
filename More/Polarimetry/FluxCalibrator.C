#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/*! When true, the FluxCalibrator constructor will first calibrate the
  the flux calibrator observations using the off-pulse polarization to
  compute the arbitrary polarimetric boost.  Not yet implemented.  */
bool Pulsar::FluxCalibrator::self_calibrate = false;

Pulsar::FluxCalibrator::FluxCalibrator ()
{
  calculated = false;
}

Pulsar::FluxCalibrator::FluxCalibrator (const vector<Archive*>& archs)
{
  if (archs.size()==0)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator",
		 "error empty Pulsar::Archive vector");
  
  calculated = false;

  for (unsigned iarch=0; iarch<archs.size(); ++iarch)
    add_observation (archs[iarch]);
}

void Pulsar::FluxCalibrator::add_observation (const Archive* archive)
{
  if ( archive->get_type() != Signal::FluxCalOn &&
       archive->get_type() != Signal::FluxCalOff )

    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "Pulsar::Archive='" + archive->get_filename() + "'"
		 "is not a FluxCal");

  string reason;
  if (calibrator && !calibrator->calibrator_match (archive, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
		 "Pulsar::Archive='" + calibrator->get_filename() + "'"
		 "\ndoes not mix with '" + archive->get_filename() + "'"
		 "\n" + reason);

  unsigned nchan = archive->get_nchan ();

  if (!calibrator) {

    calibrator = archive->clone();

    mean_ratio_on.resize (nchan);
    mean_ratio_off.resize (nchan);

  }
  else if (calibrator->get_type() != Signal::FluxCalOn &&
	   archive->get_type() == Signal::FluxCalOn)
    // Keep the FPTM naming convention in which the
    // Pulsar::FluxCalibrator is named for the first on-source
    // observation
    calibrator = archive->clone();


  const Pulsar::Archive* arch = archive;

  if (archive->get_state () != Signal::Intensity) {
    Pulsar::Archive* clone = arch->clone();
    clone->convert_state (Signal::Intensity);
    arch = clone;
  }

  const Pulsar::Integration* integration = arch->get_Integration(0);

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  if (verbose) 
    cerr << "Pulsar::FluxCalibrator call Pulsar::Integration::cal_levels "
	   << archive->get_filename() << endl;

  integration->cal_levels (cal_hi, cal_lo);
  Estimate<double> unity(1.0);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    // Take the ratio of the total intensity
    Estimate<double> ratio = cal_hi[0][ichan]/cal_lo[0][ichan] - unity ;
      
    if (arch->get_type() == Signal::FluxCalOn)
      mean_ratio_on[ichan] += ratio;
    else if (arch->get_type() == Signal::FluxCalOff)
      mean_ratio_off[ichan] += ratio;
      
  }

  filenames.push_back (archive->get_filename());
  calculated = false;
}


//! Calibrate the flux in the given archive
void Pulsar::FluxCalibrator::calibrate (Archive* arch)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  string reason;
  if (!calibrator->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + reason);

  create (arch->get_nchan());

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    calibrate (arch->get_Integration(isub));
}


void Pulsar::FluxCalibrator::create (unsigned required_nchan)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  unsigned nchan = calibrator->get_nchan ();

  if (!required_nchan)
    required_nchan = nchan;

  if (calculated && cal_flux.size() == required_nchan)
    return;


  ratio_on.resize (nchan);
  ratio_off.resize (nchan);
  
  for (unsigned ichan=0; ichan<nchan; ++ichan) {
    ratio_on[ichan] = mean_ratio_on[ichan].get_Estimate();
    ratio_off[ichan] = mean_ratio_off[ichan].get_Estimate();
  }

  if (ratio_on.size() == required_nchan) {
    calculate (ratio_on, ratio_off);
    return;
  }

  throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
	       "Cannot currently calibrate archives with different nchan");

}


void Pulsar::FluxCalibrator::calculate (vector<Estimate<double> >& on,
					vector<Estimate<double> >& off)
{
  double hyd_mJy = hydra_flux_mJy (calibrator->get_centre_frequency());
  unsigned nchan = on.size();

  cal_flux.resize (nchan);
  T_sys.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (on[ichan]==0 || off[ichan]==0) {
      cal_flux[ichan] = T_sys[ichan] = 0;
      continue;
    }

    Estimate<double> ratio_diff = 1.0/on[ichan] - 1.0/off[ichan];

    cal_flux[ichan] = hyd_mJy/ratio_diff;

    T_sys[ichan] = cal_flux[ichan]/off[ichan];

    if (cal_flux[ichan].val < sqrt(cal_flux[ichan].var)
	|| T_sys[ichan].val < sqrt(T_sys[ichan].var) ) {
      
      if (verbose)
	cerr << "Pulsar::FluxCalibrator channel=" << ichan  << ": low signal"
	  "\n\t\tratio on=" << on[ichan] << " ratio off=" << off[ichan] <<
	  "\n\t\tcal flux=" << cal_flux[ichan] <<
	  "\n\t\tsys flux=" << T_sys[ichan] << endl;
      
      cal_flux[ichan] = T_sys[ichan] = 0;
    }
    
  }  // end for each chan

  calculated = true;
}

void Pulsar::FluxCalibrator::calibrate (Integration* subint)
{
  unsigned npol = subint->get_npol();
  unsigned nchan = subint->get_nchan();

  for (unsigned ichan=0; ichan<nchan; ichan++)
    for (unsigned ipol=0; ipol<npol; ipol++)
      *(subint->get_Profile (ipol, ichan)) *= cal_flux[ichan].val;
}

//! Get the number of frequency channels in the calibrator
unsigned Pulsar::FluxCalibrator::get_nchan () const
{
  return calibrator->get_nchan();
}

//! Given the observing frequency in MHz, returns the flux of Hydra in mJy
double Pulsar::FluxCalibrator::hydra_flux_mJy (double cfreq)
{
  return pow (cfreq/1400.0, -.91) * 43.1 * 1000;
}

namespace Pulsar {

  //! FluxCalibrator parameter communication
  class FluxCalibratorInfo : public Pulsar::Calibrator::Info {
    
  public:
    //! Constructor
    FluxCalibratorInfo (const Pulsar::FluxCalibrator* cal) 
    { calibrator = cal; }
    
    //! Return the number of parameter classes
    unsigned get_nclass () const { return 2; }
    
    //! Return the name of the specified class
    const char* get_name (unsigned iclass)
    {
      switch (iclass) {
      case 0:
	return "Calibrator (Jy)";
      case 1:
	return "System (Jy)";
      default:
	return "";
      }
    }
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass)
    {
      return 1;
    }
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam)
    {
      Estimate<float> retval;
      
      if (iclass == 0)
	retval = calibrator->cal_flux[ichan];
      else if (iclass == 1)
	retval = calibrator->T_sys[ichan];
      
      // convert to Jy
      retval *= 1e-3;
      return retval;
    }
    
  protected:
    
    Reference::To<const Pulsar::FluxCalibrator> calibrator;
    
  };

}

Pulsar::Calibrator::Info* Pulsar::FluxCalibrator::get_Info () const
{
  const_cast<FluxCalibrator*>(this)->create();
  return new FluxCalibratorInfo (this);
}
