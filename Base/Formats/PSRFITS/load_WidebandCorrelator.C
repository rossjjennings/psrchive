#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "FITSError.h"

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

  if (status == 0) {
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
  if(status == 0) {
    ext->configfile = tempstr.get();
  }
  else {
    if (verbose == 3)
      cerr << FITSError (status, "FITSArchive::load_WidebandCorrelator",
			 "fits_read_key BECONFIG").get_message() << endl;
    status = 0;
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

