#include "TimerArchive.h"
#include "TimerIntegration.h"
#include "Error.h"

#include "timer++.h"
#include "coord.h"

#include <unistd.h>

//! files are big endian by default
bool Pulsar::TimerArchive::big_endian = true;

//
//
//
Pulsar::TimerArchive::TimerArchive ()
{
  if (verbose)
    cerr << "TimerArchive default constructor" << endl;

  Timer::verbose = verbose;
  Timer::init (&hdr);
  valid = false;
}

//
//
//
Pulsar::TimerArchive::TimerArchive (const TimerArchive& arch)
{
  if (verbose)
    cerr << "TimerArchive copy construct" << endl;

  Timer::init (&hdr);
  Archive::copy (arch); // results in call to TimerArchive::copy
}

//
//
//
Pulsar::TimerArchive::~TimerArchive ()
{
  if (verbose)
    cerr << "TimerArchive destructor" << endl;
}

//
//
//
const Pulsar::TimerArchive&
Pulsar::TimerArchive::operator = (const TimerArchive& arch)
{
  if (verbose)
    cerr << "TimerArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to TimerArchive::copy
  return *this;
}

//
//
//
Pulsar::TimerArchive::TimerArchive (const Archive& arch)
{
  if (verbose)
    cerr << "TimerArchive base copy construct" << endl;

  Timer::init (&hdr);
  Archive::copy (arch); // results in call to TimerArchive::copy
}

//
//
//
Pulsar::TimerArchive::TimerArchive (const Archive& arch,
				    const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "TimerArchive base extraction construct " << endl;
  
  Timer::init (&hdr);
  TimerArchive::copy (arch, subints);
}

//
//
//
void Pulsar::TimerArchive::copy (const Archive& archive, 
				 const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "TimerArchive::copy Entering" << endl;

  if (this == &archive) {
    cerr << "Returning from TimerArchive::copy early " << endl;
    return;
  } else {
    cerr << "this == &archive passed test " << endl;
  }

  Archive::copy (archive, subints);

  const TimerArchive* tarchive = dynamic_cast<const TimerArchive*>(&archive);
  if (!tarchive)
    return;

  if (verbose) cerr << "Pulsar::TimerArchive::copy copying headers " << endl;
  hdr = tarchive->hdr;

  if (verbose)
    cerr << "TimerArchive::copy check validity" << endl;

  valid = tarchive->valid;
}

//
//
//
Pulsar::TimerArchive* Pulsar::TimerArchive::clone () const
{
  if (verbose)
    cerr << "TimerArchive::clone" << endl;
  return new TimerArchive (*this);
}

//
//
//
Pulsar::TimerArchive* 
Pulsar::TimerArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose)
    cerr << "TimerArchive::extract" << endl;
  return new TimerArchive (*this, subints);
}

/*!  
  By over-riding this funciton, inherited types may re-define the type
  of Integration to which the elements of the subints vector point.
*/
Pulsar::Integration* 
Pulsar::TimerArchive::new_Integration (Integration* subint)
{
  if (verbose)
    cerr << "TimerArchive::new_Integration" << endl;

  TimerIntegration* integration;

  if (subint)
    integration = new TimerIntegration (*subint);
  else
    integration = new TimerIntegration;

  if (!integration)
    throw Error (BadAllocation, "TimerArchive::new_Integration");
  
  return integration;
}

//! Returns a pointer to a newly loaded TimerIntegration instance
Pulsar::Integration* 
Pulsar::TimerArchive::load_Integration (const char* filename, unsigned isubint)
{
  throw Error (InvalidState, "Pulsar::TimerArchive::load_Integration",
	       "not implemented");
}


char Pulsar::TimerArchive::get_telescope_code () const
{
  // if there is only one character, assume that this is the code
  if (strlen (hdr.telid) == 1)
    return hdr.telid[0];

  // Parkes by default
  char site = '7';

  if (!strncmp (hdr.telid, "PARKES", 6))
    site = '7';
  if (!strncmp (hdr.telid, "PKS", 3))
    site = '7';
  else if (!strcmp (hdr.telid, "AAT"))
    site = '5';
  else if (!strcmp(hdr.telid, "UAO"))
    site = '5';
  else if (!strcmp (hdr.telid, "ATCA"))
    site = '2';
  else if (!strcmp (hdr.telid, "DSS43"))
    site = '6';
  else if (!strcmp (hdr.telid, "GBT"))
    site = '1';

  return site;
}

void Pulsar::TimerArchive::set_telescope_code (char code)
{
  hdr.telid[0] = code;
  hdr.telid[1] = '\0';
}

Signal::Source Pulsar::TimerArchive::get_type () const
{
  switch (hdr.obstype)
    {
    case PULSAR:
      return Signal::Pulsar;
    case CAL:
      return Signal::PolnCal;
    case HYDRA:
      return Signal::FluxCalOn;
    case NHYDRA:
    case SHYDRA:
      return Signal::FluxCalOff;
    case FLUX_ON:
      return Signal::FluxCalOn;
    case FLUX_OFF:
      return Signal::FluxCalOff;
    default:
      return Signal::Unknown;
    }
}

int Pulsar::TimerArchive::hydra_obstype ()
{
  double dec = get_coordinates().dec().getDegrees();

  /* figure out the obstype */
  if      ((dec <= -9.083) && (dec > -11.083))
    return NHYDRA;
  else if ((dec <= -11.083) && (dec > -13.083))
    return HYDRA;
  else if ((dec <= -13.083) && (dec > -15.083))
    return SHYDRA;

  throw Error (InvalidRange, "TimerArchive::hydra_obstype",
	       "invalid HYDRACAL dec=%lf", dec);
}

void Pulsar::TimerArchive::set_type (Signal::Source type)
{
  switch (type)
    {
    case Signal::Pulsar:
      hdr.obstype = PULSAR;
      break;
    case Signal::PolnCal:
      hdr.obstype = CAL;
      break;
    case Signal::FluxCalOn:
      hdr.obstype = FLUX_ON;
      break;
    case Signal::FluxCalOff:
      hdr.obstype = FLUX_OFF;
      break;
    default:
      cerr << "TimerArchive::set_type warning unrecognized type="
	   << type << endl;
      hdr.obstype = -1;
      break;
    }
}

string Pulsar::TimerArchive::get_source () const
{
  return string (hdr.psrname);
}

void Pulsar::TimerArchive::set_source (const string& source)
{
  const char* name = source.c_str();

  if (name[0] == 'J')
    name ++;

  strncpy (hdr.psrname, name, PSRNAME_STRLEN);

  hdr.psrname[PSRNAME_STRLEN-1]='\0';
}

string Pulsar::TimerArchive::get_receiver () const
{
  return "unknown";
}

void Pulsar::TimerArchive::set_receiver (const string& rec)
{
  // no where to store receiver!
}

string Pulsar::TimerArchive::get_backend () const
{
  return hdr.machine_id;
}

void Pulsar::TimerArchive::set_backend (const string& backend)
{
  if (backend.length() >= MACHINE_ID_STRLEN)
    throw Error (InvalidParam, "Pulsar::TimerArchive::set_backend",
		 "length of '%s'=%d > MACHINE_ID_STRLEN=%d",
		 backend.c_str(), backend.length()+1, MACHINE_ID_STRLEN);

  strcpy (hdr.machine_id, backend.c_str());
}

//! Get the coordinates of the source
sky_coord Pulsar::TimerArchive::get_coordinates () const
{
  sky_coord coordinates;
  coordinates.ra().setradians( hdr.ra );
  coordinates.dec().setradians( hdr.dec );

  return coordinates;
}

//! Set the coordinates of the source
/*! This method calls the correct_Integrations method in order to update
  the mini header in each sub-integration */
void Pulsar::TimerArchive::set_coordinates (const sky_coord& coordinates)
{
  hdr.dec = coordinates.dec().getradians();
  hdr.ra  = coordinates.ra().getradians();

  AnglePair galactic = coordinates.getGalactic();
  hdr.l = galactic.angle1.getDegrees();
  hdr.b = galactic.angle1.getDegrees();

  correct_Integrations ();
}
 
unsigned Pulsar::TimerArchive::get_nbin () const
{
  if (hdr.nbin < 0)
    return 0;
  return hdr.nbin;
}

void Pulsar::TimerArchive::set_nbin (unsigned numbins)
{
  hdr.nbin = numbins;
}


unsigned Pulsar::TimerArchive::get_nchan () const
{
  if (hdr.nsub_band < 0)
    return 0;
  return hdr.nsub_band;
}

void Pulsar::TimerArchive::set_nchan (unsigned numchan)
{
  hdr.nsub_band = numchan;
}


unsigned Pulsar::TimerArchive::get_npol () const
{  
  if (hdr.banda.npol < 0)
    return 0;
  return hdr.banda.npol;
}

void Pulsar::TimerArchive::set_npol (unsigned numpol)
{
  hdr.banda.npol = numpol;
}

unsigned Pulsar::TimerArchive::get_nsubint () const
{
  if (hdr.nsub_int < 0)
    return 0;
  return hdr.nsub_int;
}

void Pulsar::TimerArchive::set_nsubint (unsigned nsubint)
{
  hdr.nsub_int = nsubint;
}

double Pulsar::TimerArchive::get_bandwidth () const
{
  return hdr.banda.bw;
}

void Pulsar::TimerArchive::set_bandwidth (double bw)
{
  hdr.banda.bw = bw;
}

double Pulsar::TimerArchive::get_centre_frequency () const
{
  return hdr.banda.centrefreq;
}

void Pulsar::TimerArchive::set_centre_frequency (double cf)
{
  hdr.banda.centrefreq = cf;
}

Signal::Basis Pulsar::TimerArchive::get_basis () const
{
  switch (hdr.banda.polar)
    {
    case 1:
      return Signal::Linear;
    case 0:
      return Signal::Circular;
    default:
      return Signal::Linear;
    }
}

void Pulsar::TimerArchive::set_basis (Signal::Basis type)
{
  switch (type)
    {
    case Signal::Linear:
      if (verbose)
	cerr << "Pulsar::TimerArchive::set_basis (Signal::Linear)" << endl;
      hdr.banda.polar = 1;
      break;
    case Signal::Circular:
      if (verbose)
	cerr << "Pulsar::TimerArchive::set_basis (Signal::Circular)" << endl;
      hdr.banda.polar = 0;
      break;
    default:
      hdr.banda.polar = -1;
      throw Error (InvalidParam, "Pulsar::TimerArchive::set_basis",
		   "unrecognized Basis = %d", (int) type);
    }
}

Signal::State Pulsar::TimerArchive::get_state () const
{
  // from band.h, version 1
  /* (T) 0=I,1=AA BB,2=AA,AB,BA,BB, 3=AA, 4=BB   */

  // Matthew Britton added the poln_storage_type =
  // [IQ|XY]_POLN_STORAGE variable to distinguish between coherence
  // products and stokes parameters states

  // Willem van Straten changed this so that band.correlator_mode
  // could store the polarization state in the header of the data file
  // without making any assumptions regarding the machine

  // correlator_mode == 6 now distingishes between IQUV and AA,AB,BA,BB

  // correlator_mode == 5 added to handle storage of invariant interval

  switch (hdr.banda.correlator_mode) 
    {
    case 0:
      return Signal::Intensity;
    case 1:
      return Signal::PPQQ;
    case 2:
      return Signal::Coherence;
    case 5:
      return Signal::Invariant;
    case 6:
      return Signal::Stokes;
    default:
      return Signal::Intensity;
    }
}

void Pulsar::TimerArchive::set_state (Signal::State state)
{
  switch (state) 
    {

    case Signal::Intensity:
      hdr.banda.correlator_mode = 0;
      break;

    case Signal::PPQQ:
      hdr.banda.correlator_mode = 1;
      break;

    case Signal::Coherence:
      hdr.banda.correlator_mode = 2;
      break;

    case Signal::Invariant:
      hdr.banda.correlator_mode = 5;
      break;

    case Signal::Stokes:
      hdr.banda.correlator_mode = 6;
      break;

    default:
      hdr.banda.correlator_mode = -1;
      throw Error (InvalidParam, "TimerArchive::set_state",
		   "unrecognized state=" + State2string(state));
    }
}

//! Get the centre frequency of the observation
double Pulsar::TimerArchive::get_dispersion_measure () const
{
  return hdr.dm;
}

//! Set the centre frequency of the observation
void Pulsar::TimerArchive::set_dispersion_measure (double dm)
{
  hdr.dm = dm;
}

bool Pulsar::TimerArchive::get_flux_calibrated () const
{
  return hdr.calibrated;
}

void Pulsar::TimerArchive::set_flux_calibrated (bool done)
{
  hdr.calibrated = done;
}

bool Pulsar::TimerArchive::get_poln_calibrated () const
{
  return hdr.calibrated;
}

void Pulsar::TimerArchive::set_poln_calibrated (bool done)
{
  hdr.calibrated = done;
}

bool Pulsar::TimerArchive::get_feedangle_corrected () const
{
  return hdr.corrected & FEED_CORRECTED;
}

void Pulsar::TimerArchive::set_feedangle_corrected (bool done)
{
  set_corrected (FEED_CORRECTED, done);
}

bool Pulsar::TimerArchive::get_iono_rm_corrected () const
{
  return hdr.corrected & RM_IONO_CORRECTED;
}

void Pulsar::TimerArchive::set_iono_rm_corrected (bool done)
{
  set_corrected (RM_IONO_CORRECTED, done);
}

bool Pulsar::TimerArchive::get_ism_rm_corrected () const
{
  return hdr.corrected & RM_ISM_CORRECTED;
}

void Pulsar::TimerArchive::set_ism_rm_corrected (bool done)
{
  set_corrected (RM_ISM_CORRECTED, done);
}

bool Pulsar::TimerArchive::get_parallactic_corrected () const
{
  return hdr.corrected & PARA_CORRECTED;
}

void Pulsar::TimerArchive::set_parallactic_corrected (bool done)
{
  set_corrected (PARA_CORRECTED, done);
}

bool Pulsar::TimerArchive::get_dedispersed () const
{
  return hdr.corrected & DEDISPERSED;
}

void Pulsar::TimerArchive::set_dedispersed (bool done)
{
  set_corrected (DEDISPERSED, done);
}

void Pulsar::TimerArchive::set_corrected (int code, bool done)
{
  if (done)
    hdr.corrected |= code;
  else
    hdr.corrected &= ~code;
}

void Pulsar::TimerArchive::correct_Integrations ()
{ try {
  if( get_nsubint()==0 )
    return;

  float latitude;
  float longitude;

  telescope_coordinates (&latitude, &longitude);

  TimerIntegration* subint;

  for (unsigned isub=0; isub<get_nsubint(); isub++) {

    subint = dynamic_cast<TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "TimerArchive::correct_Integrations",
		   "Integration[%d] is not a TimerIntegration", isub);

    if (subint->get_duration () <= 0.0) {
      if (verbose)
	cerr << "TimerArchive::correct_Integrations"
	  " warning empty sub-int " << isub << endl;
      continue;
    }

    // correct the mini header LST
    subint->mini.lst_start = subint->get_epoch().LST (longitude);

    // correct the mini header azimuth, zenith, and parallactic angles
    float azimuth=0, zenith=0, parallactic=0;
    if (az_zen_para (get_coordinates().ra().getRadians(),
		     get_coordinates().dec().getRadians(),
		     subint->mini.lst_start, latitude,
		     &azimuth, &zenith, &parallactic) < 0)
    
      throw Error (FailedCall, "TimerArchive::correct_Integrations",
		   "az_zen_para failed");

    subint->mini.tel_az = azimuth;
    subint->mini.tel_zen = zenith;
    subint->mini.para_angle = parallactic;

    // set the mini header version
    subint->mini.version = 1.1;

    // feed angle is unknown
    // subint->mini.feed_ang = 0.0;

    // garbage fields
    subint->mini.junk = subint->mini.junk2 = subint->mini.junk3 = 0;
    // subint->mini.pulse_phase = -99999999;
    // subint->mini.flux_A = mini.flux_B = -9999.0;

  }
}
catch (Error& error) {
  throw error += "TimerArchive::correct_Integrations";
}
}

void Pulsar::TimerArchive::correct ()
{ try {

  float latitude;
  float longitude;

  telescope_coordinates (&latitude, &longitude);

  MJD mjd = start_time();

  // correct the MJD
  hdr.mjd     = mjd.intday  ();  
  hdr.fracmjd = mjd.fracday ();

  // correct the utdate string
  mjd.datestr (hdr.utdate, 16, "%d-%m-%Y");

  // correct the LST
  hdr.lst_start = mjd.LST (longitude);

  // correct the folding period and integration length
  hdr.nominal_period = 0.0;
  hdr.dump_time = 0.0;
  hdr.sub_int_time = 0.0;

  if (get_nsubint() > 0) {
    hdr.nominal_period = get_Integration(0) -> get_folding_period();
    hdr.sub_int_time = get_Integration(0) -> get_duration();
    hdr.dump_time = hdr.sub_int_time;
  }

  // not sure what these mean
  hdr.narchive_int = 1;
  hdr.ndump_sub_int = 1;

  // correct the polyco parameters
  if (model) {
    hdr.nspan = (int) model->get_nspan();
    hdr.ncoeff = model->get_ncoeff();
  }
  else
    hdr.nspan = hdr.ncoeff = 0;

  /* General info */
  char hostname[50];
  if (gethostname (hostname,50) != 0)
    strcpy (hostname, "unkown host");

  utc_t utc_start;
  mjd.UTC (&utc_start, NULL);
  utc2str (hdr.schedule, utc_start, "yyyydddhhmmss");

  time_t now;
  now = time(NULL);
  sprintf (hdr.comment,"TimerArchive created on %s - %s",
	   hostname, ctime(&now));

  correct_Integrations();

  // data will be unloaded in the new style
  hdr.wts_and_bpass = 1;
  valid = true;
} catch (Error& error) {
  throw error += "TimerArchive::correct";
}}

