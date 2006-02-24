#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/fitsio_Backend.h"

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

  psrfits_read_backend_phase (fptr, ext, &status);
  status = 0;

  if (ext->get_name() == "WBCORR")  {

    FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();
    if (!hdr_ext)
      throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
                   "no FITSHdrExtension extension");

    int major=-1, minor=-1;

    if (sscanf (hdr_ext->hdrver.c_str(), "%d.%d", &major, &minor) != 2)
      throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
                   "could not parse header version from " + hdr_ext->hdrver);

    if (major = 1 && minor < 10)  {

      /*
	Prior to header version 1.10, the WBCORR backend at Parkes did
	not properly set the reference_epoch.
      */
      
      correct_P236_reference_epoch = true;
      if (verbose > 1)
        cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	  "  correcting reference epoch of P236 data with version " 
             << hdr_ext->hdrver << endl;
      
    }

    if (major = 1 && minor < 14 && minor > 9)  {

      /*
	Prior to header version 1.14, the WBCORR backend at Parkes
	produced cross products that were out by a scale factor of
	two. This little check applies the correction factor if it detects
	an archive that was affected by this instrumentation bug.
      */
      
      scale_cross_products = true;
      if (verbose == 3)
        cerr << "Pulsar::FITSArchive::load_header "
	  "doubling cross products of WBCORR data with version " 
             << hdr_ext->hdrver << endl;
      
    }

    if (ext->get_argument() == (Signal::Argument) 0) {

      if (verbose > 2)
        cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	  "  undefined BE_PHASE; checking " << ext->configfile; // << endl;

      static char* conj_config = getenv ("WBCCONJCFG");
      static vector<string> conj_configs;
      if (conj_config)  {
	stringfload (&conj_configs, conj_config);
	conj_config = 0; // load it only once
	if (verbose==3 && conj_configs.size()) {
	  cerr << "WBCORR conj configurations:" << endl;
	  for (unsigned i=0; i < conj_configs.size(); i++)
	    cerr << conj_configs[i] << endl;
	}
      }
      
      if (find (conj_configs.begin(), conj_configs.end(), ext->configfile)
	  != conj_configs.end())  {
	
	ext->set_argument( Signal::Conjugate );
	
	if (verbose > 1)
	  cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	    "  correcting data with version=" << hdr_ext->hdrver
	       << " config=" << ext->configfile << endl;
	
      }

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

