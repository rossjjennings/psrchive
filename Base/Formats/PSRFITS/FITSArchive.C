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
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"

#include "FITSError.h"

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
  if (verbose == 3)
    cerr << "FITSArchive default construct" << endl;
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

  reference_epoch = archive.start_time();
  
  const FITSArchive* farchive = dynamic_cast<const FITSArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose == 3)
    cerr << "FITSArchive::copy another FITSArchive" << endl;
  
  chanbw = farchive->chanbw;
  scale_cross_products = farchive->scale_cross_products;
  reference_epoch = farchive->reference_epoch;
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
    hdr_ext->start_time = reference_epoch = 0.0;
  }
  else  {
    hdr_ext->start_time = reference_epoch = MJD ((int)day, (int)sec, frac);
    if (verbose == 3)
      cerr << "Got start time: " << hdr_ext->start_time.printall() << endl;
  }
 
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading coordinates" << endl;
  
  // Read where the telescope was pointing
  
  char* tempstr = new char[FLEN_VALUE];
  
  fits_read_key (fptr, TSTRING, "COORD_MD", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading coord mode: " 
	   << error << endl;
    }
    status = 0;
  }
  
  hdr_ext->coordmode = tempstr;
  if (verbose == 3)
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
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header unknown COORD_MD"
	   << endl;
  
  if (status != 0) {
    fits_get_errstatus(status,error);
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::load_header error reading coordinate data "
	   << error << endl;
    status = 0;
  }
  else
    set_coordinates (coord);
  
  // Pulsar FITS header definiton version

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading FITS header version" << endl;
  
  fits_read_key (fptr, TSTRING, "HDRVER", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading HDRVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else 
    hdr_ext->hdrver = tempstr;
  
  if (verbose == 3)
    cerr << "Got: Version " << tempstr << endl;
  
  // File creation date
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading file creation date" << endl;

  fits_read_key (fptr, TSTRING, "DATE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->creation_date = tempstr;

  // Name of observer
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading observer name" << endl;
  
  fits_read_key (fptr, TSTRING, "OBSERVER", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading OBSERVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->observer = tempstr;
  
  if (verbose == 3)
    cerr << "Got observer: " << tempstr << endl;
  
  // Project ID
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading project ID" << endl;

  fits_read_key (fptr, TSTRING, "PROJID", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading PROJID: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    obs_ext->project_ID = tempstr;

  if (verbose == 3)
    cerr << "Got PID: " << tempstr << endl;
  
  // Telescope name
    
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading telescope name" << endl;
    
  fits_read_key (fptr, TSTRING, "TELESCOP", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
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

  if (verbose == 3)
    cerr << "Got telescope: " << obs_ext->telescope << endl;
  
  if ((obs_ext->telescope).length() == 1)
    set_telescope_code ( (obs_ext->telescope).at(0) );
  else
    set_telescope_code ( Telescope::code((obs_ext->telescope).c_str()) );
  
  // Antenna ITRF coordinates

  load_ITRFExtension (fptr);

  // Receiver parameters

  load_Receiver (fptr);

  // WidebandCorrelator parameters

  load_WidebandCorrelator (fptr);


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
    if (verbose == 3) {
      cout << "Old WBCORR header version detected..." << endl;
      cout << "Scaling cross products to compensate" << endl;
    }
  }


  // Read the name of the source

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading source name" << endl;

  fits_read_key (fptr, TSTRING, "SRC_NAME", tempstr, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key SRC_NAME");
  
  set_source ( tempstr );
  
  // Figure out what kind of observation it was

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading OBS_MODE" << endl;

  fits_read_key (fptr, TSTRING, "OBS_MODE", tempstr, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key OBSTYPE");
  
  if (strcmp(tempstr, "PSR") == 0 || strcmp(tempstr, "LEVPSR") == 0) {
    set_type ( Signal::Pulsar );
    if (verbose == 3)
      cerr << "FITSArchive::load_header using Signal::Pulsar" << endl;
  }
  else if (strcmp(tempstr, "CAL") == 0 || strcmp(tempstr, "LEVCAL") == 0) {
    
    if (get_source() == "HYDRA_O"  || get_source() == "VIRGO_O" ||
	get_source() == "0918-1205_H" || get_source() == "3C353_O") {
      set_type ( Signal::FluxCalOn );
      if (verbose == 3)
	cerr << "FITSArchive::load_header using Signal::FluxCalOn" << endl;
    }
    else if (get_source() == "HYDRA_N" || get_source() == "HYDRA_S" ||
	     get_source() == "VIRGO_N" || get_source() == "VIRGO_S" ||
	     get_source() == "0918-1005_N" || get_source() == "0918-1405_S" ||
	     get_source() == "3C353_N" || get_source() == "3C353_S") {
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
  else if (strcmp (tempstr, "PCM") == 0)
    set_type ( Signal::Calibrator );
  else if (strcmp (tempstr, "SEARCH") == 0)
    set_type ( Signal::Unknown );
  else {
    if (verbose == 3)
      cerr << "FITSArchive::load_header WARNING unknown OBSTYPE = " 
	   << tempstr <<endl;
    set_type ( Signal::Unknown );
  }
  
  if (get_type() != Signal::Pulsar && get_type() != Signal::Unknown)
    load_CalInfoExtension (fptr);

  // Track mode

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading track mode" << endl;

  fits_read_key (fptr, TSTRING, "TRK_MODE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TRK_MODE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->trk_mode = tempstr;
  



  // Read the start UT date

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start date" << endl;

  fits_read_key (fptr, TSTRING, "STT_DATE", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_date = tempstr;
  
  // Read the start UT

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start UT" << endl;

  fits_read_key (fptr, TSTRING, "STT_TIME", tempstr, comment, &status);
  if (status != 0) {
    if (verbose == 3) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_TIME: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    hdr_ext->stt_time = tempstr;
  
  // Read the start LST (in seconds)

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading start LST" << endl;

  fits_read_key (fptr, TDOUBLE, "STT_LST", &(hdr_ext->stt_lst), comment, &status);
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

    if (verbose == 3)
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
  
    if (verbose == 3)
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
    
    if (verbose == 3)
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

  if (verbose == 3)
    cerr << "FITSArchive::load_header exit" << endl;
  

  delete[] tempstr;
}
//
// End of load_header function
// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////



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

    if (verbose == 3)
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
    const WidebandCorrelator* ext = get<WidebandCorrelator>();
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

    /* This is a temporary fix until the Receiver class is fully
       implemented in every Archive-derived class */

    auto_ptr<char> tempstr ( new char[FLEN_VALUE] );
    
    if (get_basis() == Signal::Linear)
      strcpy (tempstr.get(), "LIN");
    
    else if (get_basis() == Signal::Circular)
      strcpy (tempstr.get(), "CIRC");
    
    else
      strcpy (tempstr.get(), "    ");
      
    fits_update_key (fptr, TSTRING, "FD_POLN", tempstr.get(), comment, &status);

    if (status)
      throw FITSError (status, "Pulsar::FITSArchive::unload", 
                       "fits_update_key FD_POLN");

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

  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::Agent::advocate test " << filename << endl;

  fits_open_file(&test_fptr, filename, READONLY, &status);

  if (status != 0) {

    if (Archive::verbose == 3) {
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
    
    











