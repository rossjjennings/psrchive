/***************************************************************************
 *
 *   Copyright (C) 2007 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ASPArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include <fitsio.h>
#include "FITSError.h"

using namespace std;

void Pulsar::ASPArchive::init ()
{
  // initialize the ASPArchive attributes
  // ASP-specific things we may want:
  //   - filetype version
  //   - per-channel timestamps?
  asp_file_version=0;
}

Pulsar::ASPArchive::ASPArchive()
{
  init ();
}

Pulsar::ASPArchive::~ASPArchive()
{
  // destroy any ASPArchive-specific resources
}

Pulsar::ASPArchive::ASPArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASPArchive::ASPArchive (const ASPArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive construct copy ASPArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASPArchive::ASPArchive (const Archive& arch, 
                                    const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::ASPArchive::copy (const Archive& archive, 
                                   const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy dynamic cast call" << endl;
  
  const ASPArchive* like_me = dynamic_cast<const ASPArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::copy another ASPArchive" << endl;

  // copy ASPArchive attributes
  // XXX which direction are we copying?
}

Pulsar::ASPArchive* Pulsar::ASPArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::clone" << endl;
  return new ASPArchive (*this);
}

Pulsar::ASPArchive* 
Pulsar::ASPArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose > 2)
    cerr << "Pulsar::ASPArchive::extract" << endl;
  return new ASPArchive (*this, subints);
}

void Pulsar::ASPArchive::load_header (const char* filename)
{
  // load all BasicArchive and ASPArchive attributes from filename.
  // Includes most info found in primary ASP fits header.

  // Try to open fits file
  fitsfile *f;
  int status=0; 
  int int_tmp=0;
  if (fits_open_file(&f, filename, READONLY, &status)) 
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "fits_open_file(%s)", filename);

  // Look for appropriate header version
  char ctmp[80];
  fits_movabs_hdu(f, 1, NULL, &status);
  fits_read_key(f, TSTRING, "HDRVER", ctmp, NULL, &status);
  if (status) {
    fits_close_file(f, &int_tmp);
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "Couldn't find HDRVER key");
  }

  // check version string here
  string version = ctmp;
  if (version.compare("Ver1.0")==0) { asp_file_version=ASP_FITS_V10; }
  else if (version.compare("Ver1.1")==0) { asp_file_version=ASP_FITS_V11; }
  else if (version.compare("Ver1.0.1")==0) { asp_file_version=ASP_FITS_V101; }
  else { asp_file_version=0; }

  // Unrecognized HDRVER keyword
  if (asp_file_version==0) {
    fits_close_file(f, &int_tmp);
    throw Error (InvalidState, "Pulsar::ASPArchive::load_header",
        "Unrecognized HDRVER=%s in file %s", version.c_str(), filename);
  }

  // Only Ver1.0.1 is supported currently.
  // load_header handles all versions, but load_integration only 
  // supports V101 for now.
  if (asp_file_version!=ASP_FITS_V101) {
    fits_close_file(f, &int_tmp);
    throw Error (InvalidState, "Pulsar::ASPArchive::load_header",
        "Only ASP Ver1.0.1 currently supported (version=%s)", version.c_str());
  }

  // Some tmp variables
  float flt_tmp, flt_tmp2;
  string stmp;

  // Determine number of subints
  fits_get_num_hdus(f, &int_tmp, &status);
  int_tmp -= 3; // info HDUs
  if (asp_file_version==ASP_FITS_V101) { int_tmp /= 2; }
  if (!status) set_nsubint(int_tmp);

  // Pol info always the same for ASP
  set_npol(4);
  set_state(Signal::Coherence);
  set_scale(Signal::FluxDensity);
  set_faraday_corrected(false);
  set_poln_calibrated(false);
  set_dedispersed(true);  //XXX potentially not true

  // Info from main header
  fits_movabs_hdu(f, 1, NULL, &status);
  
  // Number of bins per pulse period in folded profiles.
  fits_read_key(f, TINT, "NPTSPROF", &int_tmp, NULL, &status);
  if (!status) set_nbin(int_tmp);

  // Source name
  fits_read_key(f, TSTRING, "SRC_NAME", ctmp, NULL, &status);
  if (!status) set_source(ctmp);

  // TEMPO telescope site code
  fits_read_key(f, TSTRING, "OBSVTY", ctmp, NULL, &status);
  if (!status) set_telescope(ctmp);

  // Observation type (PSR or CAL), again see Types.h
  fits_read_key(f, TSTRING, "OBS_MODE", ctmp, NULL, &status);
  if (!status) {
    stmp = ctmp;
    if (stmp.compare("CAL")==0) {
      set_type(Signal::PolnCal);
      // TODO : check for flux cal srcs
    } else {
      set_type(Signal::Pulsar);
    }
  }

  // Approx center frequency, MHz.  
  // XXX does this key exist in all versions?
  fits_read_key(f, TFLOAT, "FSKYCENT", &flt_tmp, NULL, &status);
  if (!status) set_centre_frequency(flt_tmp);

  // Move to BECONFIG table.
  fits_movnam_hdu(f, ASCII_TBL, "BECONFIG", 0, &status);

  // Number of channels in file.
  int col=0;
  fits_get_colnum(f, CASEINSEN, "NChan", &col, &status);
  fits_read_col(f, TINT, col, 1, 1, 1, NULL, &int_tmp, NULL, &status);
  if (!status) set_nchan(int_tmp);

  // get sign of BW
  int bw_sign=1;
  fits_get_colnum(f, CASEINSEN, "CFRQ0", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  fits_get_colnum(f, CASEINSEN, "CFRQ1", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp2, NULL, &status);
  if (flt_tmp2<flt_tmp) { bw_sign=-1; }

  // Bandwidth
  fits_get_colnum(f, CASEINSEN, "BW0", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  if (!status) set_bandwidth((float)bw_sign * flt_tmp * nchan);

  // Move to COHDDISP table
  fits_movnam_hdu(f, ASCII_TBL, "COHDDISP", 0, &status);

  // Dispersion measure, pc/cm^3.
  fits_get_colnum(f, CASEINSEN, "DM", &col, &status);
  fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
  if (!status) {
    if (get_type()==Signal::Pulsar) 
      set_dispersion_measure(flt_tmp);
    else 
      set_dispersion_measure(0.0);
  }

  // Done for now
  int_tmp=0;
  fits_close_file(f, &int_tmp);

  // Catch any FITS errors
  if (status) 
    throw FITSError (status, "Pulsar::ASPArchive::load_header",
        "Error reading header values (file=%s)", filename);

}

Pulsar::Integration*
Pulsar::ASPArchive::load_Integration (const char* filename, unsigned subint)
{

  // Basic check to see if header has been loaded correctly
  if (asp_file_version!=ASP_FITS_V101) 
    throw Error (InvalidState, "Pulsar::ASPArchive::load_Integration",
        "Invalid asp_file_version (%d) or header not loaded", asp_file_version);

  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by 
  // load_header.
  resize_Integration(integration);

  // Reopen file
  fitsfile *f;
  int status=0, zstatus=0; 
  if (fits_open_file(&f, filename, READONLY, &status)) 
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
        "fits_open_file(%s)", filename);

  // tmp vars
  int int_tmp;
  float flt_tmp;

  // Figure out integration time, mjd.
  int imjd=0;
  fits_movabs_hdu(f, 1, NULL, &status); 
  fits_read_key(f, TINT, "NDUMPS", &int_tmp, NULL, &status);
  fits_read_key(f, TFLOAT, "SCANLEN", &flt_tmp, NULL, &status);
  if (!status) integration->set_duration(flt_tmp/(float)int_tmp);
  fits_read_key(f, TINT, "STT_IMJD", &imjd, NULL, &status);
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Error reading integration time or MJD", subint);
  }

  // Read in RFs
  int col=0;
  char ctmp[16];
  fits_movnam_hdu(f, ASCII_TBL, "BECONFIG", 0, &status);
  for (int i=0; i<nchan; i++) {
    sprintf(ctmp, "CFRQ%d", i);
    fits_get_colnum(f, CASEINSEN, ctmp, &col, &status);
    fits_read_col(f, TFLOAT, col, 1, 1, 1, NULL, &flt_tmp, NULL, &status);
    integration->set_centre_frequency(i, flt_tmp);
  }
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Error reading RF array", subint);
  }

  // Move to specified subint 
  if (asp_file_version==ASP_FITS_V101) { 
    sprintf(ctmp, "DUMPREF%d", subint);
    fits_movnam_hdu(f, ASCII_TBL, ctmp, 0, &status);
  } else {
    fits_movabs_hdu(f, subint+4, NULL, &status);
  }
  if (status) {
    fits_close_file(f, &zstatus);
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration", 
        "Couldn't find specified subint (%d)", subint);
  }

  // Load in ref phase/freq stamps and time for this subint
  double midsecs;
  double *midphase = new double[nchan];
  double *midper = new double[nchan];
  fits_read_key(f, TDOUBLE, "MIDSECS", &midsecs, NULL, &status);
  fits_get_colnum(f, CASEINSEN, "REFPHASE", &col, &status);
  fits_read_col(f, TDOUBLE, col, 1, 1, nchan, NULL, midphase, NULL, &status);
  fits_get_colnum(f, CASEINSEN, "REFPERIOD", &col, &status);
  fits_read_col(f, TDOUBLE, col, 1, 1, nchan, NULL, midper, NULL, &status);

  // Use avg folding period
  double pfold=0.0;
  for (int i=0; i<nchan; i++) pfold += midper[i];
  pfold /= (double)nchan;
  if (!status) integration->set_folding_period(pfold);

  // Move to data HDU
  if (asp_file_version==ASP_FITS_V101) {
    sprintf(ctmp, "ASPOUT%d", subint);
    fits_movnam_hdu(f, BINARY_TBL, ctmp, 0, &status);
  }

  // Read time offset
  double tdiff=0.0;
  if (!status) {
    fits_read_key(f, TDOUBLE, "DUMPTDIFF", &tdiff, NULL, &status);
    if (status==KEY_NO_EXIST) {
      tdiff=155.0;
      status=0;
      // warn?
    }
  }
  midsecs += tdiff*1e-9;

  // Set timestamp
  MJD epoch(imjd, midsecs/86400.0);
  integration->set_epoch(epoch);

  integration->uniform_weight(1.0);

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) {
    fits_close_file(f, &zstatus);
    delete [] midphase;
    delete [] midper;
    if (status)
      throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
          "FITS error");
    return integration;
  }

  // Load the actual data for each pol, channel from the file.
  // Still relies on column numbers... probably ok, though.
  float *data = new float[nbin]; // Temporary storage space
  int *count = new int[nbin];
  for (int ichan=0; ichan<nchan; ichan++) {
    // Load counts for this chan
    fits_read_col(f, TINT, 5*ichan+5, 1, 1, nbin, NULL, count, NULL, &status);
    for (int ipol=0; ipol<npol; ipol++) {
      // Load data for ipol, ichan 
      fits_read_col(f, TFLOAT, 5*ichan+ipol+1, 1, 1, nbin, NULL, data, NULL, 
          &status);
      // Normalize by counts
      for (int ibin=0; ibin<nbin; ibin++) data[ibin] /= (float)count[ibin];
      // Put data in integration structure:
      integration->get_Profile(ipol,ichan)->set_amps(data);
      // Rotate to align phase0 w/ epoch.
      // Need to resolve library dependencies (libpsrmore) to get this 
      // step to work.  Or find another way to do it..
      //integration->get_Profile(ipol,ichan)->rotate_phase(midphase[ichan]);
    }
  }

  // Unallocate temp space
  delete [] data;
  delete [] count;
  delete [] midphase;
  delete [] midper;

  // close file
  fits_close_file(f, &zstatus);

  // Catch any FITS errors
  if (status) 
    throw FITSError (status, "Pulsar::ASPArchive::load_Integration",
        "FITS Error");

  return integration;
}

void Pulsar::ASPArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ASPArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // This does not need to be filled in if you never need to save files
  // in this data format.  Assuming this is the case, it's best to throw 
  // an error here:
  throw Error (InvalidState, "Pulsar::ASPArchive::unload_file",
      "unload not implemented for ASPArchive");

}

string Pulsar::ASPArchive::Agent::get_description () 
{
  return "ASP Archive Version 1.0";
}

bool Pulsar::ASPArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by ASP Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    ASPArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    return false;
  }

  return false;
}

