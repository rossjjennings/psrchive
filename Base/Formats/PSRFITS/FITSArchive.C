#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "FITSError.h"
#include "genutil.h"

//! null constructor
// //////////////////////////
// //////////////////////////


void Pulsar::FITSArchive::init ()
{
  start_time.Construct("00000.00000");
  hdrver = "unset";
  creation_date = "unset";
  observer = "unset";
  project_ID = "unset";
  telescope = "unset";
  ant_x = 0.0;
  ant_y = 0.0;
  ant_z = 0.0;
  xpol_ang = 0.0;
  configfile = "unset";
  nrcvr = 0;
  coordmode = "unset";
  trk_mode = "unset";
  fd_mode = "unset";
  fa_req = 0.0;
  tcycle = 0.0;
  atten_a = 0.0;
  atten_b = 0.0;
  cal_mode = "unset";
  cal_frequency = 0.0;
  cal_dutycycle = 0.0;
  cal_phase = 0.0;
  stt_date = "unset";
  stt_time = "unset";
  stt_lst = 0.0;
  chanbw = 0.0;
  history.resize(0);
  digitiser_statistics.resize(0);
  digitiser_counts = 0;
  original_bandpass = 0;
}

Pulsar::FITSArchive::FITSArchive()
{
  init ();
}

Pulsar::FITSArchive::~FITSArchive()
{
}

Pulsar::FITSArchive::FITSArchive (const Archive& arch)
{
  if (verbose)
    cerr << "FITSArchive construct copy Archive" << endl;

  init ();
  FITSArchive::copy (arch);
}

Pulsar::FITSArchive::FITSArchive (const FITSArchive& arch)
{
  if (verbose)
    cerr << "FITSArchive construct copy FITSArchive" << endl;

  init ();
  FITSArchive::copy (arch);
}

Pulsar::FITSArchive::FITSArchive (const Archive& arch, 
				  const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "FITSArchive construct extract Archive" << endl;

  init ();
  FITSArchive::copy (arch, subints);
}

Pulsar::FITSArchive::FITSArchive (const FITSArchive& arch, 
				  const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "FITSArchive construct extract FITSArchive" << endl;

  init ();
  FITSArchive::copy (arch, subints);
}

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
  
  const FITSArchive* farchive = dynamic_cast<const FITSArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose)
    cerr << "FITSArchive::copy another FITSArchive" << endl;
  
  start_time = farchive -> start_time;
  
  hdrver = farchive -> hdrver;
  
  creation_date = farchive -> creation_date;
  observer = farchive -> observer;
  project_ID = farchive -> project_ID;
  
  configfile = farchive -> configfile;
  telescope = farchive -> telescope;
  
  cal_frequency = farchive -> cal_frequency;
  cal_dutycycle = farchive -> cal_dutycycle;
  cal_phase = farchive -> cal_phase;
}

//! Returns a pointer to a new copy-constructed FITSArchive instance
Pulsar::Archive* Pulsar::FITSArchive::clone () const
{
  if (verbose)
    cerr << "FITSArchive::clone" << endl;
  return new FITSArchive (*this);
}

//! Returns a pointer to a new select copy-constructed FITSArchive instance
Pulsar::Archive* 
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

  // /////////////////////////////////////////////////////////////////
  
  // Read start MJD  
  long day;
  long sec;
  double frac;
  char* tempstr1 = new char [FLEN_VALUE];
  
  if (verbose)
    cerr << "FITSArchive::load_header reading MJDs" << endl;
  
  fits_read_key (fptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_read_key (fptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_read_key (fptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key STT_*");
  
  start_time = MJD ((int)day, (int)sec, frac);
  
  if (verbose)
    cerr << "FITSArchive::load_header reading coordinates" << endl;

  // Read where the telescope was pointing
  fits_read_key (fptr, TSTRING, "COORD_MD", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading coord mode: " 
	   << error << endl;
    }
    status = 0;
  }
  
  coordmode = tempstr1;
  sky_coord coord;
  
  if (coordmode == "J2000") {
    char* tempstr2 = new char [FLEN_VALUE];
    fits_read_key (fptr, TSTRING, "STT_CRD1", tempstr1, comment, &status);
    fits_read_key (fptr, TSTRING, "STT_CRD2", tempstr2, comment, &status);
    coord.setHMSDMS(tempstr1,tempstr2);
  }     
  else if (coordmode == "Gal") {
    double co_ord1, co_ord2;
    AnglePair temp;
    fits_read_key (fptr, TDOUBLE, "STT_CRD1", &co_ord1, comment, &status);
    fits_read_key (fptr, TDOUBLE, "STT_CRD2", &co_ord2, comment, &status);
    temp.setDegrees(co_ord1,co_ord2);
    coord.setGalactic(temp);
  }
  else if (coordmode == "Ecliptic") {
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header Ecliptic COORD_MD not implimented" 
	   << endl;
  }
  else if (coordmode == "AZEL") {
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header AZEL COORD_MD not implimented"
	   << endl;
  }
  else if (coordmode == "HADEC") {
    if (verbose)
      cerr << "WARNING: FITSArchive::load_header HADEC COORD_MD not implimented"
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

  fits_read_key (fptr, TSTRING, "HDRVER", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading HDRVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else 
    hdrver = tempstr1;
  
  // File creation date

  if (verbose)
    cerr << "FITSArchive::load_header reading file creation date" << endl;

  fits_read_key (fptr, TSTRING, "DATE", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    creation_date = tempstr1;
  
  // Name of observer

  if (verbose)
    cerr << "FITSArchive::load_header reading observer name" << endl;

  fits_read_key (fptr, TSTRING, "OBSERVER", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading OBSERVER: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    observer = tempstr1;
  
  // Project ID

  if (verbose)
    cerr << "FITSArchive::load_header reading project ID" << endl;

  fits_read_key (fptr, TSTRING, "PROJID", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading PROJID: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    project_ID = tempstr1;
  
  // Telescope name

  if (verbose)
    cerr << "FITSArchive::load_header reading telescope name" << endl;

  fits_read_key (fptr, TSTRING, "TELESCOP", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TELESCOP: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    telescope = tempstr1;
  
  // Antenna ITRF coordinates

  if (verbose)
    cerr << "FITSArchive::load_header reading antenna location" << endl;

  fits_read_key (fptr, TDOUBLE, "ANT_X", &ant_x, comment, &status);
  fits_read_key (fptr, TDOUBLE, "ANT_Y", &ant_y, comment, &status);
  fits_read_key (fptr, TDOUBLE, "ANT_Z", &ant_z, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading ANT_X,Y,Z: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Receiver name

  if (verbose)
    cerr << "FITSArchive::load_header reading receiver" << endl;

  fits_read_key (fptr, TSTRING, "FRONTEND", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading FRONTEND: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    set_receiver(tempstr1);

  // Read the feed configuration

  if (verbose)
    cerr << "FITSArchive::load_header reading feed config" << endl;

  fits_read_key (fptr, TSTRING, "FD_POLN", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading FD_POLN: " 
	   << error << endl;
    }
    status = 0;
  }
  else {
    if (tempstr1 == "LIN")
      set_basis ( Signal::Linear );
    else if (tempstr1 == "CIRC")
      set_basis ( Signal::Circular );
    else
      if (verbose) {
	cerr << "FITSArchive::load_header WARNING unknown FD_POLN: " 
	     << error << endl;
      }
  }
  
  // Read angle of X-probe wrt platform zero

  if (verbose)
    cerr << "FITSArchive::load_header reading XPLO_ANG" << endl;

  fits_read_key (fptr, TFLOAT, "XPOL_ANG", &xpol_ang, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading XPOL_ANG: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read the name of the instrument used

  if (verbose)
    cerr << "FITSArchive::load_header reading instrument name" << endl;

  fits_read_key (fptr, TSTRING, "BACKEND", tempstr1, comment, &status);
  if(status == 0) {
    set_backend(tempstr1);
  }
  else {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading BACKEND: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read the name of the instrument configuration file (if any)

  if (verbose)
    cerr << "FITSArchive::load_header reading instrument config" << endl;

  fits_read_key (fptr, TSTRING, "BECONFIG", tempstr1, comment, &status);
  if(status == 0) {
    configfile = tempstr1;
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

  fits_read_key (fptr, TINT, "NRCVR", &nrcvr, comment, &status);
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

  fits_read_key (fptr, TSTRING, "SRC_NAME", tempstr1, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key SRC_NAME");
  
  set_source ( tempstr1 );
  
  // Figure out what kind of observation it was

  if (verbose)
    cerr << "FITSArchive::load_header reading OBS_MODE" << endl;

  fits_read_key (fptr, TSTRING, "OBS_MODE", tempstr1, comment, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_key OBSTYPE");
  
  if (strcmp (tempstr1, "PSR") == 0) {
    set_type ( Signal::Pulsar );
    if (verbose)
      cerr << "FITSArchive::load_header using Signal::Pulsar" << endl;
  }
  else if (strcmp (tempstr1, "CAL") == 0) {
    
    if (get_source().substr(0,4) == "HYDRA" || 
	get_source().substr(0,4) == "VIRGO") {
      set_type ( Signal::FluxCalOn );
      if (verbose)
	cerr << "FITSArchive::load_header using Signal::FluxCalOn" << endl;
    }
    else if (get_source().substr(0,8) == "HYDRA_OFF" ||
	     get_source().substr(0,8) == "VIRGO_OFF") {
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
  else if (strcmp (tempstr1, "SEARCH") == 0)
    set_type ( Signal::Unknown );
  else {
    if (verbose)
      cerr << "FITSArchive::load_header WARNING unknown OBSTYPE = " 
	   << tempstr1 <<endl;
    set_type ( Signal::Unknown );
  }
  
  // Read detailed CAL information

  if (verbose)
    cerr << "FITSArchive::load_header reading CAL info" << endl;
  
  if (get_type() == Signal::Pulsar || get_type() == Signal::Unknown) {
    cal_frequency = cal_dutycycle = cal_phase = 0;
  }
  else {
    fits_read_key (fptr, TFLOAT, "CAL_FREQ", &cal_frequency, comment, &status);
    fits_read_key (fptr, TFLOAT, "CAL_DCYC", &cal_dutycycle, comment, &status);
    fits_read_key (fptr, TFLOAT, "CAL_PHS", &cal_phase, comment, &status);
    
    if(status != 0) {
      if (verbose) {
	fits_get_errstatus(status,error);
	cerr << "FITSArchive::load_header WARNING reading CAL info: " 
	     << error << endl;
      }
      status = 0;
    }
  }
  
  // Track mode

  if (verbose)
    cerr << "FITSArchive::load_header reading track mode" << endl;

  fits_read_key (fptr, TSTRING, "TRK_MODE", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TRK_MODE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    trk_mode = tempstr1;
  
  // Feed track mode

  if (verbose)
    cerr << "FITSArchive::load_header reading feed track mode" << endl;

  fits_read_key (fptr, TSTRING, "FD_MODE", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading FD_MODE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    fd_mode = tempstr1;

  // Read requested feed angle

  if (verbose)
    cerr << "FITSArchive::load_header reading requested feed angle" << endl;

  fits_read_key (fptr, TFLOAT, "FA_REQ", &fa_req, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading FA_REQ: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read correlator cycle time

  if (verbose)
    cerr << "FITSArchive::load_header reading cycle time" << endl;

  fits_read_key (fptr, TDOUBLE, "TCYCLE", &tcycle, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading TCYCLE: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read attenuator settings

  if (verbose)
    cerr << "FITSArchive::load_header reading attenuator settings" << endl;

  fits_read_key (fptr, TFLOAT, "ATTEN_A", &atten_a, comment, &status);
  fits_read_key (fptr, TFLOAT, "ATTEN_B", &atten_b, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading ATTEN_A,B: " 
	   << error << endl;
    }
    status = 0;
  }
  
  // Read the start UT date

  if (verbose)
    cerr << "FITSArchive::load_header reading start date" << endl;

  fits_read_key (fptr, TSTRING, "STT_DATE", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_DATE: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    stt_date = tempstr1;
  
  // Read the start UT

  if (verbose)
    cerr << "FITSArchive::load_header reading start UT" << endl;

  fits_read_key (fptr, TSTRING, "STT_TIME", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_header WARNING reading STT_TIME: " 
	   << error << endl;
    }
    status = 0;
  }
  else
    stt_time = tempstr1;
  
  // Read the start LST (in seconds)

  if (verbose)
    cerr << "FITSArchive::load_header reading start LST" << endl;

  fits_read_key (fptr, TDOUBLE, "STT_LST", &stt_lst, comment, &status);
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
  
  // Move to the Processing History HDU, where only values in the 
  // last row (most recent) of each column are extracted
  
  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_movnam_hdu HISTORY");
  
  // Get the number of rows in the binary table
  
  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  history.resize(numrows);
  
  for (int i = 0; i < numrows; i++) {
    history[i] = new proc_hist();
    history[i]->load(fptr, i+1);
  }
  
  set_nbin (history[numrows-1]->nbin);
  set_npol (history[numrows-1]->npol);
  set_centre_frequency (history[numrows-1]->ctr_freq);
  set_nchan (history[numrows-1]->nchan);

  chanbw = history[numrows-1]->chanbw;
  
  string polstr = history[numrows-1]->pol_type;
  
  if(polstr == "XXYY") {
    set_state ( Signal::PPQQ );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::PPQQ" << endl;
  }
  else if(polstr == "STOKE") {
    set_state ( Signal::Stokes );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Stokes" << endl;
  }
  else if(polstr == "XXYYCRCI") {
    set_state ( Signal::Coherence );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Coherence" << endl;
  }
  else if(polstr == "INTEN") {
    set_state ( Signal::Intensity );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Intensity" << endl;
  }
  else if(polstr == "INVAR")
    set_state ( Signal::Invariant );
  else {
    if (verbose) {
      cerr << "FITSArchive:load_header WARNING unknown POL_TYPE = " 
	   << polstr <<endl;
      cerr << "FITSArchive:load_header setting Signal::Intensity" 
	   << endl;
    }
    set_state ( Signal::Intensity );
  }  
  
  set_bandwidth(get_nchan()*chanbw);
  
  if(history[numrows-1]->rm_corr == 1) {
    set_ism_rm_corrected (true);
    set_iono_rm_corrected (true);
  }
  else if(history[numrows-1]->rm_corr == 0) {
    set_ism_rm_corrected (false);
    set_iono_rm_corrected (false);
  }
  else {
    if (verbose) {
      cerr << "FITSArchive:load_header unexpected value in RM_CORR flag" 
	   << endl;
    }
    set_iono_rm_corrected (false);
    set_ism_rm_corrected (false);
  }
  
  if (verbose)
    cerr << "FITSArchive::load_header WARNING rotation measure ambiguity" 
	 << endl;
  
  //
  // The Pulsar::Archive class has two rotation measure correction
  // flags, one for the ionosphere and one for the interstellar
  // medium. The FITS definition lumps them both together. This isn't
  // really a problem in the loader, because if the FITS correction
  // flag is set, both the Pulsar::Archive flags will need to be set.
  //
  // Worth noting though.
  //  

  if(history[numrows-1]->par_corr == 1) {
    set_parallactic_corrected (true);
    set_feedangle_corrected (false);
  }
  else if(history[numrows-1]->par_corr == 0) {
    set_feedangle_corrected (false);
    set_parallactic_corrected (false);
  }
  else {
    if (verbose) {
      cerr << "FITSArchive::load_header unexpected PAR_CORR flag" 
	   << endl;
    }
    set_parallactic_corrected (false);
    set_feedangle_corrected(false);
  }
  
  if (verbose)
    cerr << "FITSArchive::load WARNING, assuming PA_CORR implies FA_CORR"
	 << endl;
  
  if(history[numrows-1]->dedisp == 1)
    set_dedispersed (true);
  else if(history[numrows-1]->dedisp == 0)
    set_dedispersed (false);
  else {
    if (verbose) {
      cerr << "FITSArchive::load unexpected DEDISP flag" 
	   << endl;
    }
    set_dedispersed (false);
  }
  
  if (verbose)
    cerr << "FITSArchive::load_header finished with processing history" 
	 << endl;
  
  // Move to the DIG_STAT Header Data Unit
  
  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_movnam_hdu DIG_STAT");
  
  // Get the number of rows (ie. the number of sub-ints)
  
  numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  digitiser_statistics.resize(numrows);
  
  for (int i = 0; i < numrows; i++) {
    digitiser_statistics[i] = new digistat();
    digitiser_statistics[i]->load(fptr, i+1);
  }
  
#if 0
  
  // Move to the DIG_CNTS Header Data Unit
  
  digitiser_counts = new digicount();
  digitiser_counts->load(fptr);
  
#endif
  
  // Move to the BANDPASS Header Data Unit
  
  original_bandpass = new bandpass();
  original_bandpass->load(fptr, nrcvr);
  
  // Load the NSITE code from the polyco

  int colnum = 0;
  long rownum = 0;
  int initflag = 0;
  
  if (verbose)
    cerr << "FITSArchive::load_header reading NSITE from polyco" 
	 << endl;
  
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_movnam_hdu POLYCO");

  fits_get_num_rows (fptr, &rownum, &status);
  
  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  
  static char* nullstr = strdup(" ");
  char* the_code = new char;
  
  fits_read_col (fptr, TSTRING, colnum, rownum, 1, 1, nullstr, 
                 &the_code, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_read_col TZRSITE");

  set_telescope_code(*the_code);

  // Load the ephemeris from the FITS file
  
  ephemeris.load(fptr);
  
  if (verbose)
    cerr << "FITSArchive::load_header ephemeris loaded" 
	 << endl;
  
  // Set the dispersion measure
  
  double dispmeas = ephemeris.get_dm();
  set_dispersion_measure(dispmeas);
  
  // Load the polyco from the FITS file
  
  model.load(fptr);
  
  if (verbose)
    cerr << "FITSArchive::load_header polyco loaded" 
	 << endl;
  
  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_header", 
		     "fits_movnam_hdu SUBINT");
  
  // Get the number of rows (ie. the number of sub-ints)
  
  numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  set_nsubint(numrows);
  
  if (verbose)
    cerr << "FITSArchive::load_header there are " << numrows << " subints"
	 << endl;
  
  // Finished with the file for now
  
  fits_close_file(fptr,&status);
  
  if (verbose)
    cerr << "FITSArchive::load_header finished" 
	 << endl;
  
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
  
  char* tempstr1 = new char [FLEN_VALUE];
  char error[FLEN_ERRMSG];
  char* comment = 0;
  
  // Read the feed configuration
  fits_read_key (sfptr, TSTRING, "FD_POLN", tempstr1, comment, &status);
  if (status != 0) {
    if (verbose) {
      fits_get_errstatus(status,error);
      cerr << "FITSArchive::load_Integration WARNING reading FD_POLN: " 
	   << error << endl;
    }
    status = 0;
  }
  else {
    if (tempstr1 == "LIN")
      integ->set_basis ( Signal::Linear );
    else if (tempstr1 == "CIRC")
      integ->set_basis ( Signal::Circular );
    else
      if (verbose) {
	cerr << "FITSArchive::load_Integration WARNING unknown FD_POLN: " 
	     << error << endl;
      }
  }
  
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
    integ -> set_state ( Signal::PPQQ );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::PPQQ" << endl;
  }
  else if(polstr == "STOKE") {
    integ -> set_state ( Signal::Stokes );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Stokes" << endl;
  }
  else if(polstr == "XXYYCRCI") {
    integ -> set_state ( Signal::Coherence );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Coherence" << endl;
  }
  else if(polstr == "INTEN") {
    integ -> set_state ( Signal::Intensity );
    if (verbose)
      cerr << "FITSArchive:load_Integration setting Signal::Intensity" << endl;
  }
  else if(polstr == "INVAR")
    integ -> set_state ( Signal::Invariant );
  else {
    if (verbose) {
      cerr << "FITSArchive:load_Integration WARNING unknown POL_TYPE = " 
	   << polstr <<endl;
      cerr << "FITSArchive:load_Integration setting Signal::Intensity" 
	   << endl;
    }
    integ -> set_state ( Signal::Intensity );
  }
  
  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (sfptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu SUBINT");
  
  // Set the centre frequency
  
  integ -> set_centre_frequency ( get_centre_frequency() );

  // Set the observation bandwidth

  integ -> set_bandwidth ( get_bandwidth() );
  
  // Set the dispersion measure
  
  integ -> set_dispersion_measure ( get_dispersion_measure() );

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
  
  newmjd = start_time + time;
  
  // Set the folding period

  integ -> set_folding_period (model.period(newmjd));

  // Correct for a possible phase offset

  Phase stt_phs = model.phase(start_time);
  Phase off_phs = model.phase(newmjd);

  double diff = off_phs.fracturns() - stt_phs.fracturns();

  if (verbose) {
    
    cerr << "I think the phase of the start time is "
	 << stt_phs.fracturns() << " fracturns" << endl;
    
    cerr << "I think the phase of the integration mid time is "
	 << off_phs.fracturns() << " fracturns" << endl;
    
    cerr << "I think there is an offset of " << diff
	 << " fractional turns between the phase of the start time"
	 << "and the phase of the mid-time in integration " 
	 << isubint << endl;

  }
  
  newmjd -= diff * integ -> get_folding_period();
  
  if (verbose) {
    
    cerr << "I think the folding period is " << integ -> get_folding_period()
	 << " seconds" << endl;
    
    cerr << "Subtracting an offset of " << diff * integ -> get_folding_period()
	 << " seconds to correct for the phase offset" << endl;
    
  }

  integ -> set_epoch (newmjd);
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration epoch " 
	 << newmjd << endl;

  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  double duration = 0.0;
  
  fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &duration, &initflag, &status);
  
  integ -> set_duration (duration);
  
  // Set up the data vector, only Pulsar::Archive base class is friend

  resize (integ);

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
    integ -> set_weight(j, weights[j]);
  
  // Load the channel centre frequencies
  
  if (verbose)
    cerr << "Pulsar::FITSArchive::load_Integration reading channel freqs" 
	 << endl;
  
  counter = 1;
  vector < float >  chan_freqs(get_nchan());
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		 chan_freqs.begin(), &initflag, &status);
  
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
      integ -> set_frequency (j, get_centre_frequency()
			      -0.5*(get_bandwidth()+chanbw)+j*chanbw);
    }
  }
  else
    for (unsigned j = 0; j < get_nchan(); j++) {
      //integ -> set_frequency(j, chan_freqs[j]);
      integ -> set_frequency(j, chan_freqs[j]);
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
		   scales[a].begin(), &initflag, &status);
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
		   offsets[a].begin(), &initflag, &status);
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
      
      p = integ -> get_Profile(a,b);
      
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
      }

      p -> set_amps(fltarray);
      p -> set_state(polmeas);
      
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
    cerr << "FITSArchive::unload_file (" << filename << ")" << endl;

  fitsfile* myfptr = 0;

  // status returned by FITSIO routines
  int status = 0;

  // Standard string length defined in fitsio.h
  char error[FLEN_ERRMSG];

  // To create a new file we need a FITS file template to provide the format

  char* psrhome = getenv ("PSRFITSDEFN");
  if (!psrhome)
    throw Error (FailedCall,
  		 "FITSArchive::unload_file", "PSRFITSDEFN not defined");
 
  if (verbose) {
    cerr << "FITSArchive::unload_file creating file " 
	 << filename << endl;
    cerr << "FITSArchive::unload_file using template "
	 << psrhome << endl;
  }

  string clobbername = "!";
  clobbername += filename;

  if (verbose)
    cerr << "FITSArchive::unload_file call fits_create_template ("
         << clobbername << ", " << psrhome << ")" << endl;

  fits_create_template (&myfptr, clobbername.c_str(), psrhome, &status);
  
  if (status)
    throw FITSError (status, "FITSArchive::unload_file", "fits_create_file");

  // Start writing the data now that our file pointer
  // should point to something
  
  // do not return comments in fits_read_key
  char* comment = 0;

  // Write the source name

  string source = get_source().c_str();
  
  fits_update_key (myfptr, TSTRING, "SRC_NAME", (char*)source.c_str(), comment, &status);

  // Write MJD info
      
  long day = (long)start_time.intday();
  long sec = (long)start_time.get_secs();
  double frac = start_time.get_fracsec();

  fits_update_key (myfptr, TLONG, "STT_IMJD", &day, comment, &status);
  fits_update_key (myfptr, TLONG, "STT_SMJD", &sec, comment, &status);
  fits_update_key (myfptr, TDOUBLE, "STT_OFFS", &frac, comment, &status);
  
  if(status != 0) {
    fits_get_errstatus(status,error);
    if (verbose)
      cerr << "WARNING: FITSArchive::unload_file MJD - " << error << endl;
    status = 0;
  }
  
  // Write pulsar position data
  
  fits_update_key (myfptr, TSTRING, "COORD_MD", (char*)coordmode.c_str(), 
		   comment, &status);
  
  AnglePair newcoord;
  
  if (coordmode == "J2000") {
    
    newcoord = get_coordinates().getRaDec();
    
    string strHMS = newcoord.angle1.getHMS();
    string strDMS = newcoord.angle2.getDMS();
    
    fits_update_key (myfptr, TSTRING, "STT_CRD1", (char*)strHMS.c_str(), 
		     comment, &status);
    fits_update_key (myfptr, TSTRING, "STT_CRD2", (char*)strDMS.c_str(), 
		     comment, &status); 
  }
  else if (coordmode == "Gal") {
    
    newcoord = get_coordinates().getGalactic();
    
    char l_deg[32];
    char b_deg[32];
    
    sprintf(l_deg, "%f", newcoord.angle1.getDegrees());
    sprintf(b_deg, "%f", newcoord.angle2.getDegrees());
    
    fits_update_key (myfptr, TSTRING, "STT_CRD1", l_deg, 
		     comment, &status);
    fits_update_key (myfptr, TSTRING, "STT_CRD2", b_deg, 
		     comment, &status); 
  }
  else if (coordmode == "Ecliptic") {
    if (verbose) {
      cerr << "WARNING: FITSArchive::unload_file Ecliptic COORD_MD not implimented"
	   << endl;
    }
  }
  else if (coordmode == "AZEL") {
    if (verbose) {
      cerr << "WARNING: FITSArchive::unload_file AZEL COORD_MD not implimented"
	   << endl;
    }
  }
  else if (coordmode == "HADEC") {
    if (verbose) {
      cerr << "WARNING: FITSArchive::unload_file HADEC COORD_MD not implimented"
	   << endl;
    }
  }
  else
    if (verbose) {
      cerr << "WARNING: FITSArchive::unload_file unknown COORD_MD"
	   << endl;
    }
  
  // Write other parameters
  
  if (get_basis() == Signal::Linear) {
    char* useful = new char[4];
    sprintf(useful, "%s", "LIN");
    fits_update_key (myfptr, TSTRING, "FD_POLN", 
		     useful, comment, &status);
  }
  else if (get_basis() == Signal::Circular) {
    char* useful = new char[4];
    sprintf(useful, "%s", "CIRC");
    fits_update_key (myfptr, TSTRING, "FD_POLN", 
		     useful, comment, &status);
  }
  else {
    char* useful = new char[4];
    sprintf(useful, "%s", "    ");
    fits_update_key (myfptr, TSTRING, "FD_POLN", 
		     useful, comment, &status);
  }
  
  fits_update_key (myfptr, TSTRING, "HDRVER", 
		   (char*)hdrver.c_str(), comment, &status);

  fits_update_key (myfptr, TSTRING, "DATE", 
		   (char*)creation_date.c_str(), comment, &status);
  
  fits_update_key (myfptr, TSTRING, "OBSERVER", 
		   (char*)observer.c_str(), comment, &status);

  fits_update_key (myfptr, TSTRING, "PROJID", 
		   (char*)project_ID.c_str(), comment, &status);

  fits_update_key (myfptr, TSTRING, "TELESCOP", 
  		   (char*)telescope.c_str(), comment, &status);
  
  fits_update_key (myfptr, TDOUBLE, "ANT_X", (double*)&ant_x, 
		   comment, &status);
  fits_update_key (myfptr, TDOUBLE, "ANT_Y", (double*)&ant_y, 
		   comment, &status);
  fits_update_key (myfptr, TDOUBLE, "ANT_Z", (double*)&ant_z, 
		   comment, &status);
  
  fits_update_key (myfptr, TFLOAT, "XPOL_ANG", (float*)&xpol_ang, 
		   comment, &status);

  fits_update_key (myfptr, TSTRING, "FRONTEND", 
  		   (char*)get_receiver().c_str(), comment, &status);
  
  fits_update_key (myfptr, TSTRING, "BACKEND", 
		   (char*)get_backend().c_str(), comment, &status);
  
  fits_update_key (myfptr, TSTRING, "BECONFIG", 
		   (char*)configfile.c_str(), comment, &status);
  
  fits_update_key (myfptr, TINT, "NRCVR", (int*)&nrcvr, comment, &status);
  
  fits_update_key (myfptr, TSTRING, "TRK_MODE", 
  		   (char*)trk_mode.c_str(), comment, &status);
  
  fits_update_key (myfptr, TSTRING, "FD_MODE", 
  		   (char*)fd_mode.c_str(), comment, &status);
  
  fits_update_key (myfptr, TFLOAT, "FA_REQ", (float*)&fa_req, 
		   comment, &status);
  
  fits_update_key (myfptr, TDOUBLE, "TCYCLE", (double*)&tcycle, 
		   comment, &status);
  
  fits_update_key (myfptr, TFLOAT, "ATTEN_A", (float*)&atten_a, 
		   comment, &status);
  fits_update_key (myfptr, TFLOAT, "ATTEN_B", (float*)&atten_b, 
		   comment, &status);
   
  fits_update_key (myfptr, TSTRING, "CAL_MODE", 
  		   (char*)cal_mode.c_str(), comment, &status);
  
  fits_update_key (myfptr, TSTRING, "STT_DATE", 
  		   (char*)stt_date.c_str(), comment, &status);

  fits_update_key (myfptr, TSTRING, "STT_TIME", 
  		   (char*)stt_time.c_str(), comment, &status);
  
  fits_update_key (myfptr, TDOUBLE, "STT_LST", (double*)&stt_lst, 
		   comment, &status);
  
  string tempstr3;
  
  if (get_type() == Signal::Pulsar)
    tempstr3 = "PSR";
  else if (get_type() == Signal::PolnCal)
    tempstr3 = "CAL";
  else if (get_type() == Signal::FluxCalOn)
    tempstr3 = "CAL";
  else if (get_type() == Signal::FluxCalOff)
    tempstr3 = "CAL";
  else
    tempstr3 = "UNKNOWN";
  
  fits_update_key (myfptr, TSTRING, "OBS_MODE", 
		   (char*)tempstr3.c_str(), comment, &status);
  
  float tempfloat = 0.0;

  tempfloat = cal_frequency;
  fits_update_key (myfptr, TFLOAT, "CAL_FREQ", 
		   &tempfloat, comment, &status);

  tempfloat = cal_dutycycle;
  fits_update_key (myfptr, TFLOAT, "CAL_DCYC", 
		   &tempfloat, comment, &status);

  tempfloat = cal_phase;
  fits_update_key (myfptr, TFLOAT, "CAL_PHS", 
		   &tempfloat, comment, &status);
        
  if (verbose)
    cerr << "FITSArchive::unload_file finished in primary header" << endl;
  
  // Finished with primary header information
  
  // /////////////////////////////////////////////////////////////////
    
  // Move to the Processing History HDU and set more information
  
  fits_movnam_hdu (myfptr, BINARY_TBL, "HISTORY", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_file",
		     "fits_movnam_hdu HISTORY");
  
  proc_hist* latest = new proc_hist();
  
  latest->nbin = get_nbin();
  latest->ctr_freq = get_centre_frequency();
  latest->npol = get_npol();
  latest->nchan = get_nchan();
  latest->chanbw = get_bandwidth()/get_nchan();
  
  if(get_state() == Signal::PPQQ)
    sprintf(latest->pol_type,"XXYY");
  else if(get_state() == Signal::Stokes)
    sprintf(latest->pol_type,"STOKE");
  else if(get_state() == Signal::Coherence)
    sprintf(latest->pol_type,"XXYYCRCI");
  else if(get_state() == Signal::Intensity)
    sprintf(latest->pol_type,"INTEN");
  else if(get_state() == Signal::Invariant)
    sprintf(latest->pol_type,"INVAR");
  else {
    if (verbose)
      cerr << "WARNING: FITSArchive::unload_file unexpected poln state" 
	   << endl;
    sprintf(latest->pol_type,"INVAL");
  }
  
  if (verbose)
    cerr << "FITSArchive::unload_file setting flags" << endl;
  
  // Set some true/false flags
      
  if (verbose)
    cerr << "FITSArchive::unload_file setting true/false flags" << endl;
  
  // WARNING! There is ambiguity here due to the different ways in which
  // rotation measure correction flags are stored
  
  latest->rm_corr = truthval(get_ism_rm_corrected());
  latest->par_corr = truthval(get_parallactic_corrected());
  latest->dedisp = truthval (get_dedispersed());
  
  latest->nbin_prd = get_nbin();
  latest->tbin = ((get_Integration(0)->get_folding_period())/(double(get_nbin())));
  
  time_t thetime;
  time(&thetime);
  
  sprintf(latest->date_pro, "%s", asctime(gmtime(&thetime)));
  sprintf(latest->proc_cmd, "%s", "PSRCHIVE SCHEME");
  
  // Add another row to the processing history
  
  fits_delete_rows (myfptr, 1, 1, &status);
  
  fits_insert_rows (myfptr, 0, history.size()+1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_file", 
		     "fits_insert_rows HISTORY");
  
  for (unsigned i = 0; i < history.size(); i++) {
    history[i]->unload(myfptr, i+1);
  }

  latest->unload(myfptr, history.size()+1);

  if (verbose) {
    cerr << "FITSArchive::unload_file finished in processing history" 
	 << endl;
  }
  
  // Write the ephemeris to the FITS file
  
  ephemeris.unload(myfptr);
  
  if (verbose)
    cerr << "FITSArchive::unload_file ephemeris written" << endl;

  // Write the polyco to the FITS file
  
  model.unload(myfptr);
  
  if (verbose)
    cerr << "FITSArchive::unload_file polyco written" << endl;
  
  // Unload some of the other HDU's

  // Move to the DIG_STAT Header Data Unit
  
  fits_movnam_hdu (myfptr, BINARY_TBL, "DIG_STAT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_file", 
		     "fits_movnam_hdu DIG_STAT");
  
  //long oldrownum = 0;
  //fits_get_num_rows (myfptr, &oldrownum, &status);
  //fits_delete_rows (myfptr, 1, oldrownum, &status);
  
  fits_insert_rows (myfptr, 0, digitiser_statistics.size(), &status);
  
  for (unsigned i = 0; i < digitiser_statistics.size(); i++) {
    digitiser_statistics[i]->unload(myfptr, i+1);
  }
  
#if 0
  
  // Move to the DIG_CNTS Header Data Unit
  
  digitiser_counts->unload(myfptr);
  
#endif
  
  // Move to the BANDPASS Header Data Unit
  
  original_bandpass->unload(myfptr, nrcvr);
  
  // Now write the actual integrations to file

  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (myfptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_file", 
		     "fits_movnam_hdu SUBINT");
      
  // Delete all information in the data HDU to ensure
  // no conflicts with the new state

  long oldrownum = 0;

  fits_get_num_rows (myfptr, &oldrownum, &status);

  fits_delete_rows (myfptr, 1, oldrownum, &status);

  fits_insert_rows (myfptr, 0, nsubint, &status);

  if (verbose) {
    long newrownum = 0;
    fits_get_num_rows (myfptr, &newrownum, &status);
    if (verbose) {
      cerr << "FITSArchive::unload_file DATA row count = "
	   << newrownum
	   << endl;
    }
  }

  // Set the sizes of the columns which may have changed
  
  int colnum = 0;
  
  fits_get_colnum (myfptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (myfptr, colnum, nchan, &status);
  
  if (verbose)
    cerr << "FITSArchive::unload_file DAT_FREQ resized to "
	 << nchan
	 << endl;

  fits_get_colnum (myfptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (myfptr, colnum, nchan, &status);

  if (verbose)
    cerr << "FITSArchive::unload_file DAT_WTS resized to "
	 << nchan
	 << endl;

  fits_get_colnum (myfptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  fits_modify_vector_len (myfptr, colnum, nchan*npol, &status);

  if (verbose)
    cerr << "FITSArchive::unload_file DAT_OFFS resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (myfptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  fits_modify_vector_len (myfptr, colnum, nchan*npol, &status);

  if (verbose)
    cerr << "FITSArchive::unload_file DAT_SCL resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (myfptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (myfptr, colnum, nchan*npol*nbin, &status);

  if (verbose)
    cerr << "FITSArchive::unload_file DATA resized to "
	 << nchan*npol*nbin
	 << endl;
  
  // Iterate over all rows, calling the unload_integration function to
  // fill in the next spot in the file.
  
  for(unsigned i = 0; i < nsubint; i++)
    unload_integration(i+1, get_Integration(i), myfptr);

  if (verbose)
    cerr << "FITSArchive::unload_file loaded all subintegrations" << endl;

  fits_close_file(myfptr,&status);

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
  // Set the start time of the integration

  int colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  double time = 0.0;
  time = (integ -> get_epoch () - start_time).in_seconds();

  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &time, &status);
  
  if (verbose)
    cerr << "FITSArchive::unload_integration OFFS_SUB set" << endl;


  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "TSUBINT", &colnum, &status);
 
  double duration = integ -> get_duration();
  
  fits_write_col (thefptr, TDOUBLE, colnum, row, 1, 1, &duration, &status);
  

  // Write the profile weights

  int counter = 1;
  vector < float >  weights(nchan);

  for(unsigned j = 0; j < nchan; j++)
    weights[j] = integ -> get_weight(j);

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  //fits_modify_vector_len (thefptr, colnum, nchan, &status);
  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  weights.begin(), &status);

  
  // Write the channel centre frequencies

  counter = 1;
  vector < float >  chan_freqs(nchan);

  for(unsigned j = 0; j < nchan; j++)
    chan_freqs[j] = integ -> get_frequency(j);

  colnum = 0;
  fits_get_colnum (thefptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  //fits_modify_vector_len (thefptr, colnum, nchan, &status);
  fits_write_col (thefptr, TFLOAT, colnum, row, 1, nchan, 
		  chan_freqs.begin(), &status);

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

  // WvS memory leak found 25/10/02: temparray1 is re-assigned by get_amps
  const float* temparray1 = 0;

  int16* temparray2 = new int16 [nbin];

  float min = 0.0;
  float max = 0.0;
  float offset = 0.0;
  float scalefac = 0.0;  
  
  bool save_signed = true;

  float max_short;

  if (save_signed)
    max_short = pow(2,15)-1.0;
  else
    max_short = pow(2,16)-1.0;

  for(unsigned a = 0; a < npol; a++) {
    for(unsigned b = 0; b < nchan; b++) {
     
      p = integ -> get_Profile(a,b);
      temparray1 = p -> get_amps();

      if (verbose)
	cerr << "FITSArchive::unload_integration got profile" << endl;

      minmaxval(nbin, (float*)temparray1, &min, &max);

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
	scalefac = max_short / (max - min);
      
      if (verbose)
	cerr << "FITSArchive::unload_integration scalefac = "
	     << scalefac
	     << endl;
      
      // Apply the scale factor
      
      for (unsigned i = 0; i < nbin; i++) {
	temparray2[i] = int16 ((temparray1[i]-offset) * scalefac);
      }

      // Write the scale factor to file

      if (verbose)
	cerr << "FITSArchive::unload_integration writing scale fac" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_SCL", &colnum, &status);
      
      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
		      &scalefac, &status);

      // Write the offset to file

      if (verbose)
	cerr << "FITSArchive::unload_integration writing offset" << endl;

      colnum = 0;
      fits_get_colnum (thefptr, CASEINSEN, "DAT_OFFS", &colnum, &status);

      fits_write_col (thefptr, TFLOAT, colnum, row, counter1, 1, 
		      &offset, &status);

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
  
    }	  
  }

  if (verbose)
    cerr << "FITSArchive::unload_integration finished" << endl;
  
}

//
// End of unload_integration function
// //////////////////////////////////
// //////////////////////////////////


string Pulsar::FITSArchive::Agent::get_description () 
{
  return "PSRFITS version 1.1";
}

// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
/*! The method tests whether or not the given file is of FITS type. */
bool Pulsar::FITSArchive::Agent::advocate (const char* filename)
{
  fitsfile* test_fptr;
  int status = 0;
  char error[FLEN_ERRMSG];

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
    
    











