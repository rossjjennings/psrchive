/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pulsar.h"

#include "FITSError.h"

using namespace std;

/*!
  \param ffptr reference to the PSRFITS file to be modified
  \param hdu_name name of the HDU to remove from ffptr
*/
void Pulsar::FITSArchive::delete_hdu (fitsfile* ffptr, const char* name)
{
  int status = 0;

  fits_movnam_hdu (ffptr, BINARY_TBL, const_cast<char*>(name), 0, &status);
  if (status != 0)
  {
    FITSError error (status, "FITSArchive::delete_hdu", 
		     "fits_movnam_hdu %s", name);
    warning << "Pulsar::FITSArchive::delete_hdu WARNING " 
            << error.get_message() << endl 
            << "Please ensure that PSRFITS template is current" << endl;
    return;
  }

  fits_delete_hdu (ffptr, 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::delete_hdu", 
		     "fits_delete_hdu %s", name);

  if (verbose == 3)
    cerr << "FITSArchive::delete_hdu " << name << " HDU deleted" << endl;
}
