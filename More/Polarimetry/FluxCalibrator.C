#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/FluxCalibratorDatabase.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/*! When true, the FluxCalibrator constructor will first calibrate the
  the flux calibrator observations using the off-pulse polarization to
  compute the arbitrary polarimetric boost.  Not yet implemented.  */
bool Pulsar::FluxCalibrator::self_calibrate = false;

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::FluxCalibrator::FluxCalibrator (const Archive* archive)
{
  calculated = false;

  if (!archive)
    return;

  const FluxCalibratorExtension* fe = archive->get<FluxCalibratorExtension>();
  if (fe) {
    // store the calibrator archive
    calibrator = archive;
    extension = fe;
    filenames.push_back( archive->get_filename() );

    cal_flux = fe->cal_flux;
    T_sys = fe->T_sys;
    calculated = true;
  }
  else
    add_observation (archive);
}

Pulsar::FluxCalibrator::FluxCalibrator (const vector<const Archive*>& archs)
{
  if (archs.size()==0)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator",
		 "error empty Pulsar::Archive vector");
  
  calculated = false;

  for (unsigned iarch=0; iarch<archs.size(); ++iarch)
    add_observation (archs[iarch]);
}

double Pulsar::FluxCalibrator::meanTsys ()
{
  double retval = 0.0;
  
  for (unsigned i = 0; i < T_sys.size(); i++) {
    retval += T_sys[i].val;
  }
  
  return (retval / T_sys.size());
}

double Pulsar::FluxCalibrator::Tsys (unsigned ichan)
{
  if (ichan > T_sys.size()-1)
    return 0.0;
  
  return (T_sys[ichan].val);
}

void Pulsar::FluxCalibrator::add_observation (const Archive* archive)
{
  if (!archive)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::add_observation",
                 "invalid Pulsar::Archive pointer");

  // done as a first step to clean up archive on exit
  Reference::To<const Archive> arch = archive;

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
  string filename = archive->get_filename ();
  bool rename_calibrator = false;

  if (!calibrator) {

    calibrator = archive->clone();

    mean_ratio_on.resize (nchan);
    mean_ratio_off.resize (nchan);

  }
  else if (calibrator->get_type() != Signal::FluxCalOn &&
                 arch->get_type() == Signal::FluxCalOn)  {

    // Keep the FPTM naming convention in which the
    // Pulsar::FluxCalibrator is named for the first on-source
    // observation
    calibrator = archive->clone();
    rename_calibrator = true;

  }

  if (archive->get_state () != Signal::Intensity) {

    if (verbose)
      cerr << "Pulsar::FluxCalibrator::add_observation clone total intensity"
           << endl;

    Pulsar::Archive* clone = arch->clone();
    clone->convert_state (Signal::Intensity);

    /* If there was no other Reference::To the input Archive*, 
       it will be deleted in the following step */

    arch = clone;

  }

  const Pulsar::Integration* integration = arch->get_Integration(0);

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  if (verbose) 
    cerr << "Pulsar::FluxCalibrator call Integration::cal_levels" << endl;

  integration->cal_levels (cal_hi, cal_lo);
  Estimate<double> unity(1.0);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (integration->get_weight(ichan) == 0)
      continue;

    // Take the ratio of the total intensity
    Estimate<double> ratio = cal_hi[0][ichan]/cal_lo[0][ichan] - unity ;
    if (arch->get_type() == Signal::FluxCalOn)
      mean_ratio_on[ichan] += ratio;
    else if (arch->get_type() == Signal::FluxCalOff)
      mean_ratio_off[ichan] += ratio;
      
  }

  if (rename_calibrator)
    filenames.insert (filenames.begin(), filename);
  else
    filenames.push_back (filename);

  calculated = false;

}

//! Set the database containing flux calibrator information
void Pulsar::FluxCalibrator::set_database (const FluxCalibratorDatabase* d)
{
  database = d;
}

//! Calibrate the flux in the given archive
void Pulsar::FluxCalibrator::calibrate (Archive* arch)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  if (arch->get_scale() != Signal::ReferenceFluxDensity)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::calibrate", 
                 "Archive scale != ReferenceFluxDensity");

  string reason;
  if (!calibrator->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + reason);

  create (arch->get_nchan());

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    calibrate (arch->get_Integration(isub));

  arch->set_scale(Signal::Jansky);
}


void Pulsar::FluxCalibrator::create (unsigned required_nchan)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::calibrate",
		 "no FluxCal Archive");

  unsigned nchan = calibrator->get_nchan ();

  if (!required_nchan)
    required_nchan = nchan;

  if (extension && extension->get_nchan() != required_nchan)
    throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
		 "required nchan=%d != extension nchan=%d",
		 required_nchan, extension->get_nchan());

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
try {
  
  if (!database) {
    if (verbose)
      cerr << "Pulsar::FluxCalibrator::calculate using default database"<<endl;
    database = new FluxCalibratorDatabase;
  }

  if (verbose)
    cerr << "Pulsar::FluxCalibrator::calculate search for source=" 
	 << calibrator->get_source() << endl;

  FluxCalibratorDatabase::Entry entry;
  entry = database->match (calibrator->get_source(),
			   calibrator->get_centre_frequency());

  if (verbose)
    cerr << "Pulsar::FluxCalibrator::calculate found matching source=" 
	 << entry.source_name[0]<< endl;

  unsigned nchan = on.size();
  
  cal_flux.resize (nchan);
  T_sys.resize (nchan);

  const Integration* subint = calibrator->get_Integration(0);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (on[ichan]==0 || off[ichan]==0) {
      cal_flux[ichan] = T_sys[ichan] = 0;
      continue;
    }

    Estimate<double> ratio_diff = 1.0/on[ichan] - 1.0/off[ichan];

    double frequency = subint->get_centre_frequency(ichan);
    double source_mJy = entry.get_flux_mJy (frequency);

    if (verbose)
      cerr << "Pulsar::FluxCalibrator::calculate channel=" << ichan << 
	" freq=" << frequency << " flux=" << source_mJy << endl;

    cal_flux[ichan] = source_mJy/ratio_diff;

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
catch (Error& error) {
  throw error += "Pulsar::FluxCalibrator::calculate";
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
    const char* get_name (unsigned iclass) const
    {
      switch (iclass) {
      case 0:
	return "\\fiC\\d0\\u\\fn (Jy)";
      case 1:
	return "\\fiT\\dsys\\u\\fn (Jy)";
      default:
	return "";
      }
    }
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const
    {
      return 1;
    }
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const
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

Pulsar::Calibrator::Type Pulsar::FluxCalibrator::get_type () const
{
  return Flux;
}

Pulsar::CalibratorExtension*
Pulsar::FluxCalibrator::new_Extension () const
{
  const_cast<FluxCalibrator*>(this)->create();
  return new FluxCalibratorExtension (this);
}

