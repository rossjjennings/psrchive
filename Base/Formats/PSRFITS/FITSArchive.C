#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/FITSSubintExtension.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/CalInfoExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/BackendExtension.h"
#include "Pulsar/DigitiserStatistics.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/Passband.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"

#include "FITSError.h"
#include "genutil.h"

#include "Telescope.h"

//! null constructor
// //////////////////////////
// //////////////////////////


void Pulsar::FITSArchive::init ()
{
  chanbw = 0.0; 
  scale_cross_products = false;
}

//
//
//
Pulsar::FITSArchive::FITSArchive()
{
  if (verbose)
    cerr << "FITSArchive default construct" << endl;
  init ();
}

//
//
//
Pulsar::FITSArchive::FITSArchive (const FITSArchive& arch)
{
  if (verbose)
    cerr << "FITSArchive copy construct" << endl;

  init ();
  Archive::copy (arch); // results in call to FITSArchive::copy
}

//
//
//
Pulsar::FITSArchive::~FITSArchive()
{
  if (verbose)
    cerr << "FITSArchive destruct" << endl;
}

//
//
//
const Pulsar::FITSArchive&
Pulsar::FITSArchive::operator = (const FITSArchive& arch)
{
  if (verbose)
    cerr << "FITSArchive assignment operator" << endl;

  Archive::copy (arch); // results in call to FITSArchive::copy
  return *this;
}

//
//
//
Pulsar::FITSArchive::FITSArchive (const Archive& arch)
{
  if (verbose)
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
  if (verbose)
    cerr << "FITSArchive base extraction construct" << endl;

  init ();
  FITSArchive::copy (arch, subints);
}


/*! The Integration subset can contain anywhere between none and all of
   integrations in the source Archive */
void Pulsar::FITSArchive::copy (const Archive& archive, 
				const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "FITSArchive::copy" << endl;

  if (this == &archive)
    return;
  
  Archive::copy (archive, subints);

  if (verbose)
    cerr << "FITSArchive::copy dynamic cast call" << endl;

  reference_epoch = archive.start_time();
  
  const FITSArchive* farchive = dynamic_cast<const FITSArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose)
    cerr << "FITSArchive::copy another FITSArchive" << endl;
  
  chanbw = farchive->chanbw;
  scale_cross_products = farchive->scale_cross_products;
  reference_epoch = farchive->reference_epoch;
}

//! Returns a pointer to a new copy-constructed FITSArchive instance
Pulsar::FITSArchive* Pulsar::FITSArchive::clone () const
{
  if (verbose)
    cerr << "FITSArchive::clone" << endl;

  return new FITSArchive (*this);
}

//! Returns a pointer to a new select copy-constructed FITSArchive instance
Pulsar::FITSArchive* 
Pulsar::FITSArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose)
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
  
  // Open the data file  
  if (verbose)
    cerr << "FITSArchive::load_header fits_open_file (" << filename << ")" 
	 << endl;
  
  fits_open_file(&fptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_open_file(%s)", filename);

  // These Extensions must exist in order to load

  add_extension (new ObsExtension);
  add_extension (new FITSHdrExtension);
  add_extension (new BackendExtension);

  ObsExtension*      obs_ext  = get<ObsExtension>();
  FITSHdrExtension*  hdr_ext  = get<FITSHdrExtension>();
  BackendExtension*  be_ext   = get<BackendExtension>();

  // /////////////////////////////////////////////////////////////////
  
  // Read start MJD  
  long day;
  long sec;
  double frac;
  
  if (verbose)
    cerr << "FITSArchive::load_header reading MJDs" << endl;
  
  fits_read_key (fptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_read_key (fptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_read_key (fptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key STT_*");
  
  hdr_ext->start_time = reference_epoch = MJD ((int)day, (int)sec, frac);
  
  if (verbose)
    cerr << "Got start time: " << hdr_ext->start_time.printall() << endl;
  
  if (verbose)
    cerr << "FITSArchive::load_header reading coordinates" << endl;
  
  // Read where the telescope was pointing
  
  char* tempstr = new char[FLEN_VALUE];
  
  fits_read_key (fptr, TSTRING, "COORD_MD", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading coord mode: " 
	   << error << endl;
    }
    status = 0;
  }
  
  hdr_ext->coordmode = tempstr;
  if (verbose)
    cerr << "Got coordinate type: " << tempstr << endl;

  sky_coord coord;
  
  if (hdr_ext->coordmode == "J2000") {

    fits_read_key (fptr, TSTRING, "STT_CRD1", tempstr, comment, &status);
    string hms = tempstr;

    fits_read_key (fptr, TSTRING, "STT_CRD2", tempstr, comment, &status);
    coord.setHMSDMS(hms.c_str(),tempstr);
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
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header Ecliptic COORD_MD not implemented" 
	   << endl;
  }
  else if (hdr_ext->coordmode == "AZEL") {
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header AZEL COORD_MD not implemented"
	   << endl;
  }
  else if (hdr_ext->coordmode == "HADEC") {
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header HADEC COORD_MD not implemented"
	   << endl;
  }
  else
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header unknown COORD_MD"
	   << endl;
  
  if (status != 0) {
    fits_get_errstatus(status,error);
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header error reading coordinate data "
	   << error << endl;
    status = 0;
  }
  else
    set_coordinates (coord);
  
  // Pulsar FITS header definiton version

  if (verbose)
    cerr << "FITSArchive::load_header reading FITS header version" << endl;
  
  fits_read_key (fptr, TSTRING, "HDRVER", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading HDRVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else 
    hdr_ext->hdrver = tempstr;
  
  if (verbose)
    cerr << "Got: Version " << tempstr << endl;
  
  // File creation date
  
  if (verbose)
    cerr << "FITSArchive::load_header reading file creation date" << endl;

  fits_read_key (fptr, TSTRING, "DATE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->creation_date = tempstr;

  // Name of observer
  
  if (verbose)
    cerr << "FITSArchive::load_header reading observer name" << endl;
  
  fits_read_key (fptr, TSTRING, "OBSERVER", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading OBSERVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->observer = tempstr;
  
  if (verbose)
    cerr << "Got observer: " << tempstr << endl;
  
  // Project ID
  
  if (verbose)
    cerr << "FITSArchive::load_header reading project ID" << endl;

  fits_read_key (fptr, TSTRING, "PROJID", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading PROJID: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->project_ID = tempstr;

  if (verbose)
    cerr << "Got PID: " << tempstr << endl;
  
  // Telescope name
    
  if (verbose)
    cerr << "FITSArchive::load_header reading telescope name" << endl;
    
  fits_read_key (fptr, TSTRING, "TELESCOP", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TELESCOP: " 
           << error << endl;
    }
    status = 0;
  }
  else {
    string mystr = tempstr;
    obs_ext->telescope = mystr.substr(mystr.find_first_not_of(" ",0),
				      mystr.length());
  }

  if (verbose)
    cerr << "Got telescope: " << obs_ext->telescope << endl;
  
  if ((obs_ext->telescope).length() == 1)
    set_telescope_code ( (obs_ext->telescope).at(0) );
  else
    set_telescope_code ( Telescope::code((obs_ext->telescope).c_str()) );
  
  // Antenna ITRF coordinates

  load_ITRFExtension (fptr);

  // Receiver parameters

  load_Receiver (fptr);

  // Read the name of the instrument used

  if (verbose)
    cerr << "FITSArchive::load_header reading instrument name" << endl;

  fits_read_key (fptr, TSTRING, "BACKEND", tempstr, comment, &status);
  if(status == 0) {
    set_backend(tempstr);
  }
  else {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading BACKEND: " 
	   << error << endl;
    }
    status = 0;
  }
  
  /////////////////////////////////////////////////////////////////////////
  /*
    Prior to header version 1.14, the WBCORR backend at Parkes
    produced cross products that were out by a scale factor of
    two. This little check applies the correction factor if it detects
    an archive that was affected by this instrumentation bug.
    
    Although it is messy and highly specific, please do not remove
    this block of code as it ensures data consistency.
  */

  int  major_number = 9;
  float minor_number = 0.9;
  int index = (hdr_ext->hdrver).find_first_of(".",0);
  sscanf((hdr_ext->hdrver.substr(0,index)).c_str(), "%d", &major_number);
  sscanf((hdr_ext->hdrver.substr(index+1,hdr_ext->hdrver.length())).c_str(), 
	 "%f", &minor_number);
  if ((strcmp(tempstr, "WBCORR") == 0) && ((major_number == 1) &&
						 (minor_number < 14))) {
    scale_cross_products = true;
    if (verbose) {
      cout << "Old WBCORR header version detected..." << endl;
      cout << "Scaling cross products to compensate" << endl;
    }
  }

  /////////////////////////////////////////////////////////////////////////

  // Read the name of the instrument configuration file (if any)

  if (verbose)
    cerr << "FITSArchive::load_header reading instrument config" << endl;

  fits_read_key (fptr, TSTRING, "BECONFIG", tempstr, comment, &status);
  if(status == 0) {
    be_ext->configfile = tempstr;
  }
  else {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading BECONFIG: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read the number of receiver channels
  
  if (verbose)
    cerr << "FITSArchive::load_header reading NRCVR" << endl;
  
  fits_read_key (fptr, TINT, "NRCVR", &(be_ext->nrcvr), comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading NRCVR: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read the name of the source

  if (verbose)
    cerr << "FITSArchive::load_header reading source name" << endl;

  fits_read_key (fptr, TSTRING, "SRC_NAME", tempstr, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key SRC_NAME");
  
  set_source ( tempstr );
  
  // Figure out what kind of observation it was

  if (verbose)
    cerr << "FITSArchive::load_header reading OBS_MODE" << endl;

  fits_read_key (fptr, TSTRING, "OBS_MODE", tempstr, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key OBSTYPE");
  
  if (strcmp(tempstr, "PSR") == 0 || strcmp(tempstr, "LEVPSR") == 0) {
    set_type ( Signal::Pulsar );
    if (verbose)
      cerr << "FITSArchive::load_header using Signal::Pulsar" << endl;
  }
  else if (strcmp(tempstr, "CAL") == 0 || strcmp(tempstr, "LEVCAL") == 0) {
    
    if (get_source() == "HYDRA_O"  || get_source() == "VIRGO_O" ||
	get_source() == "0918-1205_H" || get_source() == "3C353_O") {
      set_type ( Signal::FluxCalOn );
      if (verbose)
	cerr << "FITSArchive::load_header using Signal::FluxCalOn" << endl;
    }
    else if (get_source() == "HYDRA_N" || get_source() == "HYDRA_S" ||
	     get_source() == "VIRGO_N" || get_source() == "VIRGO_S" ||
	     get_source() == "0918-1005_N" || get_source() == "0918-1405_S" ||
	     get_source() == "3C353_N" || get_source() == "3C353_S") {
      set_type ( Signal::FluxCalOff );
      if (verbose)
	cerr << "FITSArchive::load_header using Signal::FluxCalOff" << endl;
    }
    else {
      set_type ( Signal::PolnCal );
      if (verbose)
	cerr << "FITSArchive::load_header using Signal::PolnCal" << endl;
    }
  }
  else if (strcmp (tempstr, "PCM") == 0)
    set_type ( Signal::Calibrator );
  else if (strcmp (tempstr, "SEARCH") == 0)
    set_type ( Signal::Unknown );
  else {
    if (verbose)
      cerr << "FITSArchive::load_header WARNING unknown OBSTYPE = " 
	   << tempstr <<endl;
    set_type ( Signal::Unknown );
  }
  
  if (get_type() != Signal::Pulsar && get_type() != Signal::Unknown)
    load_CalInfoExtension (fptr);

  // Track mode

  if (verbose)
    cerr << "FITSArchive::load_header reading track mode" << endl;

  fits_read_key (fptr, TSTRING, "TRK_MODE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TRK_MODE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->trk_mode = tempstr;
  

  // Read correlator cycle time

  if (verbose)
    cerr << "FITSArchive::load_header reading cycle time" << endl;
  
  fits_read_key (fptr, TDOUBLE, "TCYCLE", &(be_ext->tcycle), comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TCYCLE: " 
	   << error << endl;
    }
    status = 0;
  }
  

  // Read the start UT date

  if (verbose)
    cerr << "FITSArchive::load_header reading start date" << endl;

  fits_read_key (fptr, TSTRING, "STT_DATE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_date = tempstr;
  
  // Read the start UT

  if (verbose)
    cerr << "FITSArchive::load_header reading start UT" << endl;

  fits_read_key (fptr, TSTRING, "STT_TIME", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_TIME: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_time = tempstr;
  
  // Read the start LST (in seconds)

  if (verbose)
    cerr << "FITSArchive::load_header reading start LST" << endl;

  fits_read_key (fptr, TDOUBLE, "STT_LST", &(hdr_ext->stt_lst), comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_LST: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // ////////////////////////////////////////////////////////////////
  
  // Finished with primary header information   
  
  // ////////////////////////////////////////////////////////////////
  
  if (verbose)
    cerr << "FITSArchive::load_header finished with primary HDU" 
	 << endl;
  
  // Load the processing history

  load_ProcHistory (fptr);

  // Load the digitiser statistics
  
  load_DigitiserStatistics (fptr);
  
  // Load the original bandpass data

  load_Passband (fptr);

  // Load the calibrator stokes parameters
  
  load_CalibratorStokes (fptr);

  // Load the calibration model description
  
  load_PolnCalibratorExtension (fptr);

  // Load the ephemeris from the FITS file
  
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);

  if (status == 0) {

    ephemeris = new psrephem;
    ephemeris->load(fptr);
    dispersion_measure = ephemeris->get_dm();

    if (verbose)
      cerr << "FITSArchive::load_header ephemeris loaded" << endl;

  }
  else {
    ephemeris = 0;
    dispersion_measure = 0;
  }

  // Load the polyco from the FITS file
  
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);

  if (status == 0) {

    model = new polyco;
    model->load(fptr);
  
    if (verbose)
      cerr << "FITSArchive::load_header polyco loaded" << endl;
  
  }
  else
    model = 0;

  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);

  if (status == 0) {
  
    // Get the number of rows (ie. the number of sub-ints)
    
    long numrows = 0;
    fits_get_num_rows (fptr, &numrows, &status);
    
    set_nsubint(numrows);
    
    if (verbose)
      cerr << "FITSArchive::load_header there are " << numrows << " subints"
	   << endl;
    
    fits_read_key (fptr, TSTRING, "INT_TYPE", tempstr, comment, &status);
    
    if (status == 0) {
      if (strcmp(tempstr,"TIME") == 0 || strcmp(tempstr,"") == 0) {
	// Do nothing
      }
      else {
	if (strcmp(tempstr,"BINPHSPERI") == 0)
	  add_extension(new PeriastronOrder());
	else if (strcmp(tempstr,"BINPHSASC") == 0)
	  add_extension(new BinaryPhaseOrder());
	else if (strcmp(tempstr,"BINLNGPERI") == 0)
	  add_extension(new BinLngPeriOrder());
	else if (strcmp(tempstr,"BINLNGASC") == 0)
	  add_extension(new BinLngAscOrder());
	else
	  throw Error(InvalidParam, "FITSArchive::load_header",
		      "unknown ordering extension encountered");
	
	get<Pulsar::IntegrationOrder>()->resize(get_nsubint());
      }
    }
  }
  
  status = 0;
  
  // Finished with the file for now
  fits_close_file (fptr, &status);
  
  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::load_header",
		     "fits_close_file");

  if (verbose)
    cerr << "FITSArchive::load_header exit" << endl;
  

  delete[] tempstr;
}
//
// End of load_header function
// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
//! A function to read a single integration from a FITS file on disk.

Pulsar::Integration* 
Pulsar::FITSArchive::load_Integration (const char* filename, unsigned isubint)
{
  if (!filename)
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "filename unspecified");

  Pulsar::Integration* integ = new_Integration();

  int row = isubint + 1;
  
  int status = 0;  

  if (verbose) {
    cerr << "FITSArchive::load_Integration number " << isubint << endl;
  }
  
  double nulldouble = 0.0;
  float nullfloat = 0.0;
  int16 nullshort = -1;
  
  int initflag = 0;
  int colnum = 0;
  
  // Open the file

  fitsfile* sfptr = 0;
  
  if (verbose)
    cerr << "FITSArchive::load_Integration fits_open_file (" 
	 << filename << ")" << endl;
  
  fits_open_file(&sfptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_open_file(%s)", filename);
  
  char* tempstr = new char[FLEN_VALUE];
  char error[FLEN_ERRMSG];
  char* comment = 0;
  
  // Read the feed configuration
  fits_read_key (sfptr, TSTRING, "FD_POLN", tempstr, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_Integration WARNING reading FD_POLN: " 
	   << error << endl;
    }
    status = 0;
  }
  else {
    if (strcmp(tempstr,"LIN") == 0 || strcmp(tempstr,"LINEAR") == 0)
      integ->set_basis ( Signal::Linear );
    else if (strcmp(tempstr,"CIRC") == 0 || strcmp(tempstr,"CIRCULAR") == 0)
      integ->set_basis( Signal::Circular );
    else
      if (verbose) {
	cerr << "FITSArchive::load_Integration unknown FD_POLN: " 
	     << tempstr << endl;
      }
  }
  
  delete[] tempstr;

  // Get the polarisation state out of the history header
  
  fits_movnam_hdu (sfptr, BINARY_TBL, "HISTORY", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu HISTORY");
  
  // Get the number of rows in the binary table
  
  long numrows = 0;
  fits_get_num_rows (sfptr, &numrows, &status);
  
  // Get the # of the column holding poltype info
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "POL_TYPE", &colnum, &status);
  
  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  
  fits_get_coltype (sfptr, colnum, &typecode, &repeat, &width, &status);

  static char* nullstr = strdup(" ");

  char* polcode = new char [repeat];
  initflag = 0;
  fits_read_col (sfptr, TSTRING, colnum, numrows, 1, 1, &nullstr, 
		 &polcode, &initflag, &status);
      
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_read_col POL_TYPE");
  
  string polstr = polcode;
  delete [] polcode;

  if(polstr == "XXYY") {
    integ->set_state ( Signal::PPQQ );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::PPQQ" << endl;
  }
  else if(polstr == "STOKE") {
    integ->set_state ( Signal::Stokes );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Stokes" << endl;
  }
  else if(polstr == "XXYYCRCI") {
    integ->set_state ( Signal::Coherence );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Coherence" << endl;
  }
  else if(polstr == "INTEN") {
    integ->set_state ( Signal::Intensity );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Intensity" << endl;
  }
  else if(polstr == "INVAR")
    integ->set_state ( Signal::Invariant );
  else {
    if (verbose) {
      cerr << "FITSArchive:load_Integration WARNING unknown POL_TYPE = " 
	   << polstr <<endl;
      cerr << "FITSArchive:load_Integration setting Signal::Intensity" 
	   << endl;
    }
    integ->set_state ( Signal::Intensity );
  }
  
  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (sfptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu SUBINT");
  
  // Set the centre frequency
  
  integ->set_centre_frequency ( get_centre_frequency() );

  // Set the observation bandwidth

  integ->set_bandwidth ( get_bandwidth() );
  
  // Set the dispersion measure
  
  integ->set_dispersion_measure ( get_dispersion_measure() );

  // Set the start time of the integration
  
  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (sfptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  double time = 0.0;
  MJD newmjd;
  
  fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &time, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_read_col OFFS_SUB");
  
  newmjd = reference_epoch + time;
  
  // Set the folding period

  if (model) {

    integ->set_folding_period (model->period(newmjd));

    // Set the toa epoch, correcting for phase offset

    Phase stt_phs = model->phase(reference_epoch);
    Phase off_phs = model->phase(newmjd);

    Phase dphase = off_phs - stt_phs;
  
    newmjd -= dphase.fracturns() * integ->get_folding_period();
  
    integ->set_epoch (newmjd);
  
    if (verbose)
      cerr << "Pulsar::FITSArchive::load_Integration set_epoch " 
	   << newmjd << endl;
  }

  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  double duration = 0.0;
  
  fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &duration, &initflag, &status);
  
  integ->set_duration (duration);
  
  if (get<Pulsar::IntegrationOrder>()) {
    colnum = 0;
    fits_get_colnum (sfptr, CASEINSEN, "INDEXVAL", &colnum, &status);
    
    double value = 0.0;

    fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		   &value, &initflag, &status);
    
    if (status != 0)
      throw FITSError (status, "FITSArchive::load_Integration", 
		       "fits_read_col INDEXVAL");
    
    get<Pulsar::IntegrationOrder>()->set_Index(row-1,value);
  }
  
  // Load other useful info

  load_FITSSubintExtension(sfptr,row,integ);

  // Set up the data vector, only Pulsar::Archive base class is friend

  resize_Integration (integ);

  // Load the profile weights

  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading weights" 
	 << endl;
  
  int counter = 1;
  vector < float >  weights(get_nchan());
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  for(unsigned b = 0; b < get_nchan(); b++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, 1, &nullfloat, 
		   &weights[b], &initflag, &status);
    counter ++;
  }
  
  // Set the profile weights
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration setting weights" 
	 << endl;
  
  for(unsigned j = 0; j < get_nchan(); j++)
    integ->set_weight(j, weights[j]);
  
  // Load the channel centre frequencies
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading channel freqs" 
	 << endl;
  
  counter = 1;
  vector < float >  chan_freqs(get_nchan());
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		 &(chan_freqs[0]), &initflag, &status);
  
  // Set the profile channel centre frequencies
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration setting channel freqs" 
	 << endl;

  bool all_ones = true;
  for (unsigned j = 0; j < get_nchan(); j++)
    if (chan_freqs[j] != 1)
      all_ones = false;
  
  double chanbw = get_bandwidth() / get_nchan();
  
  if ( all_ones ) {
    if (verbose)
      cerr << "FITSArchive::load_Integration all frequencies unity - reseting" << endl;
    for (unsigned j = 0; j < get_nchan(); j++) {
      integ->set_frequency (j, get_centre_frequency()
			      -0.5*(get_bandwidth()+chanbw)+j*chanbw);
    }
  }
  else
    for (unsigned j = 0; j < get_nchan(); j++) {
      //integ->set_frequency(j, chan_freqs[j]);
      integ->set_frequency(j, chan_freqs[j]);
    }
  
  // Load the profile scale factors
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading scale factors" 
	 << endl;

  vector < vector < float > > scales(get_npol(),vector<float>(get_nchan()));

  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  
  counter = 1;
  for (unsigned a = 0; a < get_npol(); a++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		   &(scales[a][0]), &initflag, &status);
    counter += nchan;
  }

  // Load the profile offsets
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading offsets" 
	 << endl;
  
  vector < vector < float > > offsets(get_npol(),vector<float>(get_nchan()));
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  
  counter = 1;
  for (unsigned a = 0; a < get_npol(); a++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		   &(offsets[a][0]), &initflag, &status);
    counter += nchan;
  }

  // Load the data
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading profiles" 
	 << endl;
  
  counter = 1;
  Profile* p = 0;
  int16* temparray = new int16 [get_nbin()];
  float* fltarray = new float [get_nbin()];
  Signal::Component polmeas = Signal::None;
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DATA", &colnum, &status);
  
  typecode = 0;
  repeat = 0;
  width = 0;
  
  fits_get_coltype (sfptr, colnum, &typecode, &repeat, &width, &status);  

  for (unsigned a = 0; a < get_npol(); a++) {
    for (unsigned b = 0; b < get_nchan(); b++) {
      
      p = integ->get_Profile(a,b);
      
      fits_read_col (sfptr, TSHORT, colnum, row, counter, get_nbin(), 
		     &nullshort, temparray, &initflag, &status);
      
      if (status != 0) {
	throw FITSError (status, "FITSArchive::load_Integration",
			 "Error reading subint data"
			 " ipol=%d/%d ichan=%d/%d counter=%d",
			 a, get_npol(), b, get_nchan(), counter);
      }
      
      counter += get_nbin();
      
      for(unsigned j = 0; j < get_nbin(); j++) {
	fltarray[j] = temparray[j] * scales[a][b] + offsets[a][b];
	if (scale_cross_products) {
	  if (integ->get_state() == Signal::Coherence) {
	    if (a == 2 || a == 3)
	      fltarray[j] *= 2;
	  }
	}
      }
      
      p->set_amps(fltarray);
      p->set_state(polmeas);

    }  
  }
  
  delete [] temparray; 
  delete [] fltarray;
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration load complete" << endl;  
  
  // Finished with the file for now
  
  fits_close_file(sfptr,&status);

  return integ;
}

//
// End of load_Integration function
// ////////////////////////////////
// ////////////////////////////////

// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
//! An unload function to write FITSArchive data to a FITS file on disk.

void Pulsar::FITSArchive::unload_file (const char* filename) const
{

  if (!filename)
    throw Error (InvalidParam, "FITSArchive::unload_file", 
                 "filename unspecified");
try {

  if (verbose)
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
 
  if (verbose)
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

  if (verbose)
    cerr << "FITSArchive::unload_file call fits_create_file "
      "(" << clobbername << ")" << endl;

  fits_create_file (&fptr, clobbername.c_str(), &status);
  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_create_file (%s)", clobbername.c_str());

  if (verbose)
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

  char* telescope = const_cast<char*>( Telescope::name(get_telescope_code()) );

  fits_update_key (fptr, TSTRING, "TELESCOP", telescope, comment, &status);

  string source = get_source();
  
  fits_update_key (fptr, TSTRING, "SRC_NAME",
		   const_cast<char*>(source.c_str()), comment, &status);
    
  fits_update_key (fptr, TSTRING, "BACKEND", 
		   (char*)get_backend().c_str(), comment, &status);
  
  string coord1, coord2;

  const FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();

  if (hdr_ext) {

    if (verbose)
      cerr << "Pulsar::FITSArchive::unload_file FITSHdrExtension" << endl;

    unload (fptr, hdr_ext);
    hdr_ext->get_coord_string( get_coordinates(), coord1, coord2 );

  }

  else {

    AnglePair radec = get_coordinates().getRaDec();
    
    coord1 = radec.angle1.getHMS();
    coord2 = radec.angle2.getDMS();

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
    obs_mode = "CAL";
  else if (get_type() == Signal::FluxCalOff)
    obs_mode = "CAL";
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
    const BackendExtension* ext = get<BackendExtension>();
    if (ext) 
      unload (fptr, ext);
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

  long day = (long)(reference_epoch.intday());
  long sec = (long)(reference_epoch.get_secs());
  double frac = reference_epoch.get_fracsec();

  fits_update_key (fptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_update_key (fptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_update_key (fptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_update_key STT_MJD");

  if (verbose)
    cerr << "FITSArchive::unload_file finished in primary header" << endl;
  
  // Finished with primary header information
  
  // /////////////////////////////////////////////////////////////////
    
  // Move to the Processing History HDU and set more information
  
  const_cast<FITSArchive*>(this)->update_history();
  
  const ProcHistory* history = get<ProcHistory>();
  
  if (!history)
    throw Error (InvalidState,"Pulsar::FITSArchive::unload","no ProcHistory");

  unload (fptr, history);
  
  if (verbose)
    cerr << "FITSArchive::unload_file finished with processing history" 
	 << endl;

  // Write the ephemeris to the FITS file
  
  if (ephemeris) {

    ephemeris->set_dm(dispersion_measure);
    ephemeris->unload(fptr);

    if (verbose)
      cerr << "FITSArchive::unload_file ephemeris written" << endl;

  }
  else
    delete_hdu (fptr, "PSREPHEM");


  // Write the polyco to the FITS file

  if (model) { 

   model->unload(fptr);
  
   if (verbose)
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

  if (verbose)
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


// ////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////
//! A function to write an integration to a row in a FITS file on disk.

void Pulsar::FITSArchive::unload_integration (int row, 
					      const Integration* integ, 
					      fitsfile* thefptr) const
{
  int status = 0;
 
  bool has_alt_order = false;

  if (get<Pulsar::IntegrationOrder>()) {
    has_alt_order = true;
    if (verbose)
      cerr << "FITSArchive::unload_integration using " 
	   << get<Pulsar::IntegrationOrder>()->get_name()
	   << endl;
  }

  // Set the subint number
  
  int colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "ISUBINT", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum ISUBINT");
  
  fits_write_col (thefptr, TINT, colnum, row, 1, 1, &row, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col ISUBINT");
  
  // Set the start time of the integration
    
  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum OFFS_SUB");

  double time = 0.0;
  time = (integ->get_epoch () - reference_epoch).in_seconds();
  
  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &time, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col OFFS_SUB");
  
  if (verbose)
    cerr << "FITSArchive::unload_integration OFFS_SUB set" << endl;
  
  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum TSUBINT");
  
  double duration = integ->get_duration();  
  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &duration, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col TSUBINT");
  
  if (verbose)
    cerr << "FITSArchive::unload_integration TSUBINT set" << endl;
  
  // Write out the index values, if applicable
  if (has_alt_order) {
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "INDEXVAL", &colnum, &status);

    if (status != 0)
      throw FITSError (status, "FITSArchive:unload_integration",
		       "fits_get_colnum INDEXVAL");
    
    double value = get<Pulsar::IntegrationOrder>()->get_Index(row-1);
    fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &value, &status);
  }

  // Write other useful info
  
  const FITSSubintExtension* theExt = integ->get<FITSSubintExtension>();
  if (theExt)
    unload(thefptr,theExt,row);
  
  // Write the profile weights

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum DAT_WTS");

  //fits_modify_vector_len (thefptr, colnum, nchan, &status);

  vector < float >  weights(nchan);

  for(unsigned j = 0; j < nchan; j++)
    weights[j] = integ->get_weight(j);

  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  &(weights[0]), &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col DAT_WTS");
  
  // Write the channel centre frequencies

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_get_colnum DAT_FREQ");
  
  //fits_modify_vector_len (thefptr, colnum, nchan, &status);

  vector < float >  chan_freqs(nchan);

  for(unsigned j = 0; j < nchan; j++)
    chan_freqs[j] = integ->get_frequency(j);

  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  &(chan_freqs[0]), &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive:unload_integration",
		     "fits_write_col DAT_FREQ");

  // Start writing profiles
  
  if (verbose)
    cerr << "FITSArchive::unload_integration writing profiles" << endl;

  // Resize the FITS column arrays

  // 20/02/2003 This is unnecessary. I do it before I call the function.
  // This section is only included to help avoid confusion (including
  // my own!)

  /*
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_WTS", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DAT_SCL", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol), &status);
    
    colnum = 0;
    fits_get_colnum (thefptr, CASEINSEN, "DATA", &colnum, &status);
    fits_modify_vector_len (thefptr, colnum, (nchan*npol*nbin), &status);

  */
  
  int counter1 = 1;
  int counter2 = 1;

  const Profile *p = 0;

  int16* temparray2 = new int16 [nbin];

  float min = 0.0;
  float max = 0.0;
  float offset = 0.0;
  float scalefac = 0.0;  
  
  bool save_signed = true;

  float max_short;

  if (save_signed)
    max_short = pow(2.0,15.0)-1.0;
  else
    max_short = pow(2.0,16.0)-1.0;

  for(unsigned a = 0; a < npol; a++) {
    for(unsigned b = 0; b < nchan; b++) {
      
      p = integ->get_Profile(a,b);
      float* temparray1 = new float[nbin];
      for(unsigned j = 0; j < get_nbin(); j++)
	temparray1[j] = (p->get_amps())[j];
      
      if (scale_cross_products) {
	if (integ->get_state() == Signal::Coherence) {
	  if (a == 2 || a == 3)
	    for(unsigned j = 0; j < get_nbin(); j++)
	      temparray1[j] /= 2.0;
	}
      }
      
      if (verbose) {
	cerr << "FITSArchive::unload_integration got profile" << endl;
	cerr << "nchan = " << b << endl;
	cerr << "npol  = " << a << endl;
      }

      minmaxval(nbin, temparray1, &min, &max);

      if (save_signed)
	offset = 0.5 * (max + min);
      else
	offset = min;

      if (verbose)
	cerr << "FITSArchive::unload_integration offset = "
	     << offset
	     << endl;
      
      scalefac = 1.0;
      
      // Test for dynamic range
      if (fabs(min - max) < 0.000001) {
	if (verbose) {
	  cerr << "FITSArchive::unload_integration WARNING no range in profile"
	       << endl;
	}
      }
      else
	// Find the scale factor
	scalefac = (max - min) / max_short;
      
      if (verbose)
	cerr << "FITSArchive::unload_integration scalefac = "
	     << scalefac
	     << endl;
      
      // Apply the scale factor
      
      for (unsigned i = 0; i < nbin; i++) {
	temparray2[i] = int16 ((temparray1[i]-offset) / scalefac);
      }

      // Write the scale factor to file

      if (verbose)
	cerr << "FITSArchive::unload_integration writing scale fac" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_SCL", &colnum, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_get_colnum DAT_SCL");

      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
		      &scalefac, &status);
      
      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_write_col DAT_SCL");
      
      // Write the offset to file

      if (verbose)
	cerr << "FITSArchive::unload_integration writing offset" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_OFFS", &colnum, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_get_colnum DAT_OFFS");

      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
		      &offset, &status);
      
      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_write_col DAT_OFFS");

      counter1 ++;

      // Write the data

      if (verbose)
	cerr << "FITSArchive:unload_integration writing data" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DATA", &colnum, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_get_colnum DATA");

      fits_write_col (thefptr, TSHORT, colnum, row, counter2, nbin, 
		      temparray2, &status);

      if (status != 0)
	throw FITSError (status, "FITSArchive:unload_integration",
			 "fits_write_col DATA");

      counter2 = counter2 + nbin;
	
      if (verbose)
	cerr << "FITSArchive:unload_integration looping" << endl;
      
      delete[] temparray1;
    }	  
  }
  
  delete[] temparray2;
  
  if (verbose)
    cerr << "FITSArchive::unload_integration finished" << endl;
}

//
// End of unload_integration function
// //////////////////////////////////
// //////////////////////////////////

string Pulsar::FITSArchive::Agent::get_description () 
{
  return "PSRFITS version 1.4";
}

// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
/*! The method tests whether or not the given file is of FITS type. */
bool Pulsar::FITSArchive::Agent::advocate (const char* filename)
{
  fitsfile* test_fptr;
  int status = 0;
  char error[FLEN_ERRMSG];

  if (verbose)
    cerr << "Pulsar::FITSArchive::Agent::advocate test " << filename << endl;

  fits_open_file(&test_fptr, filename, READONLY, &status);

  if (status != 0) {

    if (Archive::verbose) {
      fits_get_errstatus (status, error);
      cerr << "FITSAgent::advocate fits_open_file: " << error << endl;
    }

    return false;
  }

  else {

    fits_close_file(test_fptr, &status);
    return true;

  }
}



//
// End of function
// ///////////////
// ///////////////

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
    
    











