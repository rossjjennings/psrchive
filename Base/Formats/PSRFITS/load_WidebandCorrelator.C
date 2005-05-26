#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"
#include "FITSError.h"
#include "string_utils.h"

void Pulsar::FITSArchive::load_WidebandCorrelator (fitsfile* fptr)
{
  // status returned by FITSIO routines
  int status = 0;
  // no comment
  char* comment = 0;

  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator" << endl;
  
  Reference::To<WidebandCorrelator> ext = new WidebandCorrelator;

  auto_ptr<char> tempstr ( new char[FLEN_VALUE] );

  // Read the name of the instrument used

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading instrument name" << endl;

  fits_read_key (fptr, TSTRING, "BACKEND", tempstr.get(), comment, &status);

  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_WidebandCorrelator",
			 "fits_read_key BACKEND").get_message() << endl;
    return;
  }

  ext->set_name(tempstr.get());

  /////////////////////////////////////////////////////////////////////////

  // Read the name of the instrument configuration file (if any)

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading instrument config" << endl;

  fits_read_key (fptr, TSTRING, "BECONFIG", tempstr.get(), comment, &status);
  if (status == 0) {
    ext->configfile = tempstr.get();
  }
  else {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_WidebandCorrelator",
			 "fits_read_key BECONFIG").get_message() << endl;
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

  if (ext->get_name() == "WBCORR")  {

    FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();
    if (!hdr_ext)
      throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
                   "no FITSHdrExtension extension");

    float version = 0.9;
    if (sscanf (hdr_ext->hdrver.c_str(), "%f", &version) != 1)
      throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
                   "could not parse header version from " + hdr_ext->hdrver);

    if (version < 1.135)  {

      scale_cross_products = true;
      if (verbose == 3)
        cerr << "Pulsar::FITSArchive::load_header "
                "doubling cross products of WBCORR data with version " 
             << hdr_ext->hdrver << endl;

    }

    static char* bad_config = getenv ("WBCBADCFG");
    static vector<string> bad_configs;
    if (bad_config)  {
      stringfload (&bad_configs, bad_config);
      bad_config = 0; // load it only once
      if (verbose==3 && bad_configs.size()) {
        cerr << "WBCORR bad configurations:" << endl;
        for (unsigned i=0; i < bad_configs.size(); i++)
          cerr << bad_configs[i] << endl;
      }
    }

    if (version < 1.265 && 
        find (bad_configs.begin(), bad_configs.end(), ext->configfile)
		!= bad_configs.end())  {

      conjugate_cross_products = true;

      // if (verbose == 3)
        cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
             "  correcting data with version=" << hdr_ext->hdrver
             << " config=" << ext->configfile; // << endl;

    }

  }

  // Read correlator cycle time

  if (verbose == 3)
    cerr << "FITSArchive::load_header reading cycle time" << endl;
  
  fits_read_key (fptr, TDOUBLE, "TCYCLE", &(ext->tcycle), comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_WidebandCorrelator",
			 "fits_read_key TCYCLE").get_message() << endl;
    status = 0;
  }
 
  // Read the number of receiver channels
  
  if (verbose == 3)
    cerr << "FITSArchive::load_header reading NRCVR" << endl;
  
  fits_read_key (fptr, TINT, "NRCVR", &(ext->nrcvr), comment, &status);
  if (status != 0) {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_WidebandCorrelator",
			 "fits_read_key NRCVR").get_message() << endl;
    status = 0;
  }

  if (status == 0)
    add_extension (ext);

}

