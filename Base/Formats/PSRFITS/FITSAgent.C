/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"

string Pulsar::FITSArchive::Agent::get_description ()
{
  return "PSRFITS version 1.4";
}

// /////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////
/*! The method tests whether or not the given file is of FITS type. */
bool Pulsar::FITSArchive::Agent::advocate (const char* filename)
{
  fitsfile* test_fptr = 0;
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

