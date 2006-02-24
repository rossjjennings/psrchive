#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/DigitiserStatistics.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/Passband.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/Telescope.h"
#include "Pulsar/fitsio_Backend.h"

#include "FITSError.h"

#include "Telescope.h"
#include "string_utils.h"
#include "ephio.h"

//! null constructor
// //////////////////////////
// //////////////////////////


void Pulsar::FITSArchive::init ()
{
  chanbw = 0.0; 

  scale_cross_products = false;
  conjugate_cross_products = false;
  correct_P236_reference_epoch = false;

  fcal_on_sources.push_back("Hydra");
  fcal_on_sources.push_back("HYDRA_O");
  fcal_on_sources.push_back("VIRGO_O");
  fcal_on_sources.push_back("3C353_O");
  fcal_on_sources.push_back("0918-1205_H");
  fcal_on_sources.push_back("J0918-1205");
  
  fcal_off_sources.push_back("Hydra_N");
  fcal_off_sources.push_back("Hydra_S");
  fcal_off_sources.push_back("HYDRA_N");
  fcal_off_sources.push_back("HYDRA_S");
  fcal_off_sources.push_back("VIRGO_N");
  fcal_off_sources.push_back("VIRGO_S");
  fcal_off_sources.push_back("3C353_N");
  fcal_off_sources.push_back("3C353_S");
  fcal_off_sources.push_back("0918-1105_N");
  fcal_off_sources.push_back("0918-1305_S");
  fcal_off_sources.push_back("0918-1005_N");
  fcal_off_sources.push_back("0918-1405_S"); 
  fcal_off_sources.push_back("J0918-1105");
  fcal_off_sources.push_back("J0918-1305");
  fcal_off_sources.push_back("J0918-1005");
  fcal_off_sources.push_back("J0918-1405");
}

//
//
//
Pulsar::FITSArchive::FITSArchive()
{
  if (verbose == 3)
    cerr << "FITSArchive default construct" << endl;

  add_extension(new FITSHdrExtension());

  init ();
}

//
//
//
Pulsar::FITSArchive::FITSArchive (const FITSArchive& arch)
{
  if (verbose == 3)
    cerr << "FITSArchive copy construct" << endl;

  init ();
  Archive::copy (arch); // results in call to FITSArchive::copy
}

//
//
//
Pulsar::FITSArchive::~FITSArchive()
{
  if (verbose == 3)
    cerr << "FITSArchive destruct" << endl;
}

//
//
//
const Pulsar::FITSArchive&
Pulsar::FITSArchive::operator = (const FITSArchive& arch)
{
  if (verbose == 3)
    cerr << "FITSArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to FITSArchive::copy
  return *this;
}

//
//
//
Pulsar::FITSArchive::FITSArchive (const Archive& arch)
{
  if (verbose == 3)
    cerr << "FITSArchive base copy construct" << endl;

  init ();
  Archive::copy (arch); // results in call to FITSArchive::copy
}

//
//
//
Pulsar::FITSArchive::FITSArchive (const Archive& arch, 
				  const vector<unsigned>& subints)
{
  if (verbose == 3)
    cerr << "FITSArchive base extraction construct" << endl;

  init ();
  FITSArchive::copy (arch, subints);
}


/*! The Integration subset can contain anywhere between none and all of
   integrations in the source Archive */
void Pulsar::FITSArchive::copy (const Archive& archive, 
				const vector<unsigned>& subints)
{
  if (verbose == 3)
    cerr << "FITSArchive::copy" << endl;

  if (this == &archive)
    return;
  
  Archive::copy (archive, subints);

  if (verbose == 3)
    cerr << "FITSArchive::copy dynamic cast call" << endl;

  const FITSArchive* farchive = dynamic_cast<const FITSArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose == 3)
    cerr << "FITSArchive::copy another FITSArchive" << endl;
  
  chanbw = farchive->chanbw;
  scale_cross_products = farchive->scale_cross_products;
  conjugate_cross_products = farchive->conjugate_cross_products;
}

//! Returns a pointer to a new copy-constructed FITSArchive instance
Pulsar::FITSArchive* Pulsar::FITSArchive::clone () const
{
  if (verbose == 3)
    cerr << "FITSArchive::clone" << endl;

  return new FITSArchive (*this);
}

//! Returns a pointer to a new select copy-constructed FITSArchive instance
Pulsar::FITSArchive* 
Pulsar::FITSArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose == 3)
    cerr << "FITSArchive::extract" << endl;
  return new FITSArchive (*this, subints);
}


// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//! Read FITS header info from a file into a FITSArchive object.
//

void Pulsar::FITSArchive::load_header (const char* filename)
{
  int status = 0;
  char error[FLEN_ERRMSG];
  
  fitsfile* fptr = 0;
  
  // do not return comments in fits_read_key
  char* comment = 0;

  auto_ptr<char> tempstr (new char[FLEN_VALUE]);

  // Open the data file  
  if (verbose == 3)
    cerr << "FITSArchive::load_header fits_open_file (" << filename << ")" 
	 << endl;
  
  fits_open_file(&fptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_open_file(%s)", filename);

  // These Extensions must exist in order to load

  add_extension (new ObsExtension);
  add_extension (new FITSHdrExtension);

  ObsExtension*      obs_ext  = get<ObsExtension>();
  FITSHdrExtension*  hdr_ext  = get<FITSHdrExtension>();

  // Pulsar FITS header definiton version

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading FITS header version" << endl;
  
  fits_read_key (fptr, TSTRING, "HDRVER", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading HDRVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else 
    hdr_ext->hdrver = tempstr.get();
  
  if (verbose == 3)
    cerr << "Got: Version " << tempstr.get() << endl;
  
  // File creation date
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading file creation date" << endl;

  fits_read_key (fptr, TSTRING, "DATE", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->creation_date = tempstr.get();

  // Name of observer
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading observer name" << endl;
  
  fits_read_key (fptr, TSTRING, "OBSERVER", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading OBSERVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->observer = tempstr.get();
  
  if (verbose == 3)
    cerr << "Got observer: " << tempstr.get() << endl;
  
  // Project ID
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading project ID" << endl;

  fits_read_key (fptr, TSTRING, "PROJID", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading PROJID: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->project_ID = tempstr.get();

  if (verbose == 3)
    cerr << "Got PID: " << tempstr.get() << endl;
  
  // Telescope name
    
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading telescope name" << endl;
    
  fits_read_key (fptr, TSTRING, "TELESCOP", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TELESCOP: " 
           << error << endl;
    }
    status = 0;
  }
  else {
    string mystr = tempstr.get();
    obs_ext->telescope = stringtok (&mystr, " ");
  }

  if (verbose == 3)
    cerr << "Got telescope: " << obs_ext->telescope << endl;
  
  if ((obs_ext->telescope).length() == 1)
    set_telescope_code ( (obs_ext->telescope).at(0) );
  else
    set_telescope_code ( ::Telescope::code((obs_ext->telescope).c_str()) );

  Telescope* telescope = getadd<Telescope>();
  telescope->set_coordinates (get_telescope_code());

  // Antenna ITRF coordinates

  load_ITRFExtension (fptr);

  // Receiver parameters

  load_Receiver (fptr);

  // WidebandCorrelator parameters

  load_WidebandCorrelator (fptr);

  // Figure out what kind of observation it was

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading OBS_MODE" << endl;

  fits_read_key (fptr, TSTRING, "OBS_MODE", tempstr.get(), comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key OBSTYPE");
  
  string obs_mode = tempstr.get();

  // Read the name of the source

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading source name" << endl;

  fits_read_key (fptr, TSTRING, "SRC_NAME", tempstr.get(), comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key SRC_NAME");
  
  set_source ( tempstr.get() );
  
  if (obs_mode == "PSR" || obs_mode == "LEVPSR") {
    set_type ( Signal::Pulsar );
    if (verbose == 3)
      cerr << "FITSArchive::load_header using Signal::Pulsar" << endl;
  }
  else if (obs_mode == "CAL" || obs_mode == "LEVCAL") {
    
    if (find(fcal_on_sources.begin(), fcal_on_sources.end(), 
	     get_source()) != fcal_on_sources.end()) {
      set_type ( Signal::FluxCalOn );
      if (verbose == 3)
	cerr << "FITSArchive::load_header using Signal::FluxCalOn" << endl;
    }
    
    else if (find(fcal_off_sources.begin(), fcal_off_sources.end(), 
		  get_source()) != fcal_off_sources.end()) {
      set_type ( Signal::FluxCalOff );
      if (verbose == 3)
	cerr << "FITSArchive::load_header using Signal::FluxCalOff" << endl;
    }
    
    else {
      set_type ( Signal::PolnCal );
      if (verbose == 3)
	cerr << "FITSArchive::load_header using Signal::PolnCal" << endl;
    }

   }

  else if (obs_mode == "FOF")
    set_type ( Signal::FluxCalOff );
  else if (obs_mode == "FON")
    set_type ( Signal::FluxCalOn );
  else if (obs_mode == "PCM")
    set_type ( Signal::Calibrator );
  else if (obs_mode == "SEARCH")
    set_type ( Signal::Unknown );
  else {
    if (verbose == 3)
      cerr << "FITSArchive::load_header WARNING unknown OBSTYPE = " 
	   << tempstr.get() <<endl;
    set_type ( Signal::Unknown );
  }
  
  // Read where the telescope was pointing
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading coordinates" << endl;
    
  fits_read_key (fptr, TSTRING, "COORD_MD", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading coord mode: " 
	   << error << endl;
    }
    status = 0;
  }
  
  hdr_ext->coordmode = tempstr.get();
  if (verbose == 3)
    cerr << "Got coordinate type: " << tempstr.get() << endl;

  sky_coord coord;
  
  if (hdr_ext->coordmode == "J2000") {

    fits_read_key (fptr, TSTRING, "STT_CRD1", tempstr.get(), comment, &status);
    string hms = tempstr.get();

    fits_read_key (fptr, TSTRING, "STT_CRD2", tempstr.get(), comment, &status);
    coord.setHMSDMS(hms.c_str(),tempstr.get());
  }     
  else if (hdr_ext->coordmode == "Gal") {
    double co_ord1, co_ord2;
    AnglePair temp;
    fits_read_key (fptr, TDOUBLE, "STT_CRD1", &co_ord1, comment, &status);
    fits_read_key (fptr, TDOUBLE, "STT_CRD2", &co_ord2, comment, &status);
    temp.setDegrees(co_ord1,co_ord2);
    coord.setGalactic(temp);
  }
  else if (hdr_ext->coordmode == "Ecliptic") {
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header Ecliptic COORD_MD not implemented" 
	   << endl;
  }
  else if (hdr_ext->coordmode == "AZEL") {
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header AZEL COORD_MD not implemented"
	   << endl;
  }
  else if (hdr_ext->coordmode == "HADEC") {
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header HADEC COORD_MD not implemented"
	   << endl;
  }
  else
    if (verbose)
      cerr << "FITSArchive::load_header WARNING unknown COORD_MD" << endl;
  
  if (status != 0) {
    fits_get_errstatus(status,error);
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header error reading coordinate data "
	   << error << endl;
    status = 0;
  }
  else
    set_coordinates (coord);
  
  if (get_type() != Signal::Pulsar && get_type() != Signal::Unknown)
    load_CalInfoExtension (fptr);

  // Track mode

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading track mode" << endl;

  fits_read_key (fptr, TSTRING, "TRK_MODE", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TRK_MODE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->trk_mode = tempstr.get();

  // Read the start UT date

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start date" << endl;

  fits_read_key (fptr, TSTRING, "STT_DATE", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_date = tempstr.get();
  
  // Read the start UT

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start UT" << endl;

  fits_read_key (fptr, TSTRING, "STT_TIME", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_TIME: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_time = tempstr.get();

  // /////////////////////////////////////////////////////////////////
  
  // Read start MJD  

  long day;
  long sec;
  double frac;
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading MJDs" << endl;
  
  fits_read_key (fptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_read_key (fptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_read_key (fptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);
  
  if (status != 0) {
    cerr << "FITSArchive::load_header WARNING no STT_*MJD" << endl;
    hdr_ext->start_time = 0.0;
  }
  else  {
    hdr_ext->start_time = MJD ((int)day, (int)sec, frac);
    if (verbose == 3)
      cerr << "Got start time: " << hdr_ext->start_time.printall() << endl;
  }

  // Read the start LST (in seconds)

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start LST" << endl;

  fits_read_key (fptr, TDOUBLE, "STT_LST", &(hdr_ext->stt_lst),
		 comment, &status);

  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_LST: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // ////////////////////////////////////////////////////////////////
  
  // Finished with primary header information   
  
  // ////////////////////////////////////////////////////////////////
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header finished with primary HDU" 
	 << endl;
  
  // Load the processing history
  load_ProcHistory (fptr);

  // Load the digitiser statistics
  load_DigitiserStatistics (fptr);
  
  // Load the original bandpass data
  load_Passband (fptr);

  // Load the flux calibrator extension
  load_FluxCalibratorExtension (fptr);

  // Load the calibrator stokes parameters
  load_CalibratorStokes (fptr);

  // Load the calibration model description
  load_PolnCalibratorExtension (fptr);

  // Load the ephemeris from the FITS file
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);

  if (status == 0 && get_type() == Signal::Pulsar) {

    ephemeris = new psrephem;
    ephemeris->load(fptr);
    set_dispersion_measure( ephemeris->get_dm() );
    set_rotation_measure( ephemeris->get_double(EPH_RM) );

    if (verbose == 3)
      cerr << "FITSArchive::load_header ephemeris loaded" << endl;

  }
  else {

    ephemeris = 0;
    set_dispersion_measure (0);
    set_rotation_measure (0);

    if (verbose == 3)
      cerr << "FITSArchive::load_header no ephemeris" << endl;

  }

  // Load the polyco from the FITS file

  status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);

  if (status == 0) {

    model = new polyco;
    model->load (fptr, &extra_polyco);
    hdr_model = new polyco (*model);

    if (verbose == 3)
      cerr << "FITSArchive::load_header polyco loaded" << endl;

  }
  else {

    hdr_model = model = 0;

    if (verbose == 3)
      cerr << "FITSArchive::load_header no polyco" << endl;

  }

  if (correct_P236_reference_epoch)
    P236_reference_epoch_correction ();

  // Move to the SUBINT Header Data Unit

  status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);

  if (status == 0) {

    // Get the number of rows (ie. the number of sub-ints)
    
    long numrows = 0;
    fits_get_num_rows (fptr, &numrows, &status);

    set_nsubint(numrows);
    
    if (verbose == 3)
      cerr << "FITSArchive::load_header there are " << numrows << " subints"
	   << endl;
    
    fits_read_key (fptr, TSTRING, "INT_TYPE", tempstr.get(), comment, &status);
    
    if (!status && strcmp(tempstr.get(),"TIME") && strcmp(tempstr.get(),"")) {
      if (strcmp(tempstr.get(),"BINPHSPERI") == 0)
	add_extension(new PeriastronOrder());
      else if (strcmp(tempstr.get(),"BINPHSASC") == 0)
	add_extension(new BinaryPhaseOrder());
      else if (strcmp(tempstr.get(),"BINLNGPERI") == 0)
	add_extension(new BinLngPeriOrder());
      else if (strcmp(tempstr.get(),"BINLNGASC") == 0)
	add_extension(new BinLngAscOrder());
      else
	throw Error(InvalidParam, "FITSArchive::load_header",
		    "unknown ordering extension encountered");
      
      get<Pulsar::IntegrationOrder>()->resize(get_nsubint());
    }

  }
  else if (verbose == 3)
    cerr << "FITSArchive::load_header no sub-integrations" << endl;

  status = 0;
  
  // Finished with the file for now
  fits_close_file (fptr, &status);
  
  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::load_header",
		     "fits_close_file");

  if (verbose == 3)
    cerr << "FITSArchive::load_header exit" << endl;
  
}

//
// End of load_header function
// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////

/* This correction applies only to pulsar observations made during the
   commissioning of the WBC.  As far as I know, this affects only P236.  

   Symptom: The pulses from different archives line up, despite the fact
   that the phases predicted by the polyco for the reference epochs do not.
   Consequently, if you try to line them up using the polyco, they do not.

   Remedy: Truncate the phase of the reference epoch to zero.
*/

void Pulsar::FITSArchive::P236_reference_epoch_correction ()
{
   FITSHdrExtension*  hdr_ext  = get<FITSHdrExtension>();

   if (!hdr_ext) {
     throw Error (InvalidParam, "FITSArchive::P236_reference_epoch_correction",
		  "No FITSHdrExtension found");
   }

  if (!model || get_type() != Signal::Pulsar)
    return;

  MJD original_reference_epoch = hdr_ext->start_time;
  Phase original_phase = model->phase(hdr_ext->start_time);

  hdr_ext->start_time = model->iphase( original_phase.Floor() );

  if(verbose == 3)
    cerr << "Pulsar::FITSArchive::P236_reference_epoch_correction"
    "\n   original reference epoch=" << original_reference_epoch <<
    "\n                      phase=" << original_phase <<
    "\n  corrected reference epoch=" << hdr_ext->start_time <<
    "\n                      phase=" << model->phase(hdr_ext->start_time)
	 << endl;  
}


// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
//! An unload function to write FITSArchive data to a FITS file on disk.

void Pulsar::FITSArchive::unload_file (const char* filename) const
{

  if (!filename)
    throw Error (InvalidParam, "FITSArchive::unload_file", 
                 "filename unspecified");
try {

  if (verbose == 3)
    cerr << "FITSArchive::unload_file (" << filename << ")" << endl
	 << "  with " << get_nextension() << " Extensions" << endl;

  fitsfile* fptr = 0;

  // status returned by FITSIO routines
  int status = 0;

  // To create a new file we need a FITS file template to provide the format

  char* template_file = getenv ("PSRFITSDEFN");
  if (!template_file)
    throw Error (FailedCall,
  		 "FITSArchive::unload_file", "PSRFITSDEFN not defined");
 
  if (verbose == 3)
    cerr << "FITSArchive::unload_file creating file " 
	 << filename << endl << "   using template " << template_file << endl;

  string clobbername = "!";
  clobbername += filename;

  /* the following three commands:

     fits_create_file
     fits_execute_template
     fits_movabs_hdu

     are equivalent to:

     fits_create_template

     except that they do not cause segmentation faults.
  */

  if (verbose == 3)
    cerr << "FITSArchive::unload_file call fits_create_file "
      "(" << clobbername << ")" << endl;

  fits_create_file (&fptr, clobbername.c_str(), &status);
  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_create_file (%s)", clobbername.c_str());

  if (verbose == 3)
    cerr << "FITSArchive::unload_file call fits_execute_template "
      "(" << template_file << ")" << endl;

  fits_execute_template (fptr, template_file, &status);
  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_execute_template (%s)", template_file);

  fits_movabs_hdu (fptr, 1, 0, &status);
  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_moveabs_hdu");


  // do not return comments in fits_read_key
  char* comment = 0;

  // Write the source name

  char* telescope;
  telescope = const_cast<char*>(::Telescope::name(get_telescope_code()));

  fits_update_key (fptr, TSTRING, "TELESCOP", telescope, comment, &status);

  string source = get_source();
  
  fits_update_key (fptr, TSTRING, "SRC_NAME",
		   const_cast<char*>(source.c_str()), comment, &status);
    
  string coord1, coord2;

  const FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();
  
  if (hdr_ext) {
    
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::unload_file FITSHdrExtension" << endl;
    
    unload (fptr, hdr_ext);
    hdr_ext->get_coord_string( get_coordinates(), coord1, coord2 );
  }
  
  else {

    AnglePair radec = get_coordinates().getRaDec();
    
    coord1 = radec.angle1.getHMS();
    coord2 = radec.angle2.getDMS();

    // If no FITSHdrExtension is present, assume J2000
    char* j2000 = "J2000";
    fits_update_key (fptr, TSTRING, "COORD_MD", j2000, comment, &status);

  }

  fits_update_key (fptr, TSTRING, "STT_CRD1",
		   const_cast<char*>(coord1.c_str()), comment, &status);
  fits_update_key (fptr, TSTRING, "STT_CRD2",
		   const_cast<char*>(coord2.c_str()), comment, &status); 

  string obs_mode;
  
  if (get_type() == Signal::Pulsar)
    obs_mode = "PSR";
  else if (get_type() == Signal::PolnCal)
    obs_mode = "CAL";
  else if (get_type() == Signal::FluxCalOn)
    obs_mode = "FON";
  else if (get_type() == Signal::FluxCalOff)
    obs_mode = "FOF";
  else if (get_type() == Signal::Calibrator)
    obs_mode = "PCM";
  else
    obs_mode = "UNKNOWN";
  
  fits_update_key (fptr, TSTRING, "OBS_MODE", 
		   const_cast<char*>(obs_mode.c_str()), comment, &status);

  {
    const ObsExtension* ext = get<ObsExtension>();
    if (ext) 
      unload (fptr, ext);
  }

  {

    const WidebandCorrelator* ext = get<WidebandCorrelator>();
    if (ext) 
      unload (fptr, ext);

    else {

      const Backend* backend = get<Backend>();

      if (backend) {

	if (verbose == 3)
	  cerr << "FITSArchive::unload " << backend->get_extension_name()
	       << " name=" << backend->get_name() << endl;

	fits_update_key (fptr, TSTRING, "BACKEND", 
			 const_cast<char*>(backend->get_name().c_str()),
			 comment, &status);

	psrfits_update_backend_phase (fptr, backend, &status);

      }

    }

  }

  {
    const ITRFExtension* ext = get<ITRFExtension>();
    if (ext) 
      unload (fptr, ext);
  }

  {
    const Receiver* ext = get<Receiver>();
    if (ext) 
      unload (fptr, ext);
  }

  {
    const CalInfoExtension* ext = get<CalInfoExtension>();
    if (ext) 
      unload (fptr, ext);
  }
  
  long day    = 0;
  long sec    = 0;
  double frac = 0.0;
 
  if (hdr_ext) {
    day = (long)(hdr_ext->start_time.intday());
    sec = (long)(hdr_ext->start_time.get_secs());
    frac = hdr_ext->start_time.get_fracsec();
  }

  fits_update_key (fptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_update_key (fptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_update_key (fptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_update_key STT_MJD");

  if (verbose == 3)
    cerr << "FITSArchive::unload_file finished in primary header" << endl;
  
  // Finished with primary header information
  
  // /////////////////////////////////////////////////////////////////
    
  // Move to the Processing History HDU and set more information
  
  const_cast<FITSArchive*>(this)->update_history();
  
  const ProcHistory* history = get<ProcHistory>();
  
  if (!history)
    throw Error (InvalidState,"Pulsar::FITSArchive::unload","no ProcHistory");

  unload (fptr, history);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload_file finished with processing history" 
	 << endl;

  // Write the ephemeris to the FITS file
  
  if (ephemeris) {

    ephemeris->set_dm(dispersion_measure);
    ephemeris->set_double (EPH_RM, rotation_measure);

    ephemeris->unload(fptr);

    if (verbose == 3)
      cerr << "FITSArchive::unload_file ephemeris written" << endl;

  }
  else
    delete_hdu (fptr, "PSREPHEM");


  // Write the polyco to the FITS file

  if (model) { 

   model->unload(fptr);
  
   if (verbose == 3)
     cerr << "FITSArchive::unload_file polyco written" << endl;

  }
  else
    delete_hdu (fptr, "POLYCO");

  // Unload some of the other HDU's

  const DigitiserStatistics* digistats = get<DigitiserStatistics>();
  if (digistats)
    unload (fptr, digistats);
  else
    delete_hdu (fptr, "DIG_STAT");

  const Passband* passband = get<Passband>();
  if (passband)
    unload (fptr, passband);
  else
    delete_hdu (fptr, "BANDPASS");

  const FluxCalibratorExtension* fce = get<FluxCalibratorExtension>();
  if (fce)
    unload (fptr, fce);
  else
    delete_hdu (fptr, "FLUX_CAL");

  const CalibratorStokes* stokes = get<CalibratorStokes>();
  if (stokes)
    unload (fptr, stokes);
  else
    delete_hdu (fptr, "CAL_POLN");

  const PolnCalibratorExtension* pce = get<PolnCalibratorExtension>();
  if (pce)
    unload (fptr, pce);
  else
    delete_hdu (fptr, "FEEDPAR");

  // Now write the actual integrations to file

  if (nsubint > 0)
    unload_integrations (fptr);
  else
    delete_hdu (fptr, "SUBINT");

  fits_close_file (fptr, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload_file",
		     "fits_close_file");

  if (verbose == 3)
    cerr << "FITSArchive::unload_file fits_close_file " << "(" << filename 
	 << ")" << " complete" << endl;
}
catch (Error& error) {
  throw error += "FITSArchive::unload_file";
}

}

// End of unload_file function
// //////////////////////////////////////////
// //////////////////////////////////////////



// !A quick little helper function for internal use.
// /////////////////////////////////////////////////

int Pulsar::FITSArchive::truthval (bool condition) const
{
  if (condition)
    return 1;
  else
    return 0;
}

// EOF
// ///
    
    











