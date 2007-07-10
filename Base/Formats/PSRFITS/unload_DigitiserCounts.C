/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserCounts.h"
#include "psrfitsio.h"

using namespace std;



void 
Pulsar::FITSArchive::unload (fitsfile* fptr, const DigitiserStatistics* dstats)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_digistat entered" << endl;
  
  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_CNTS", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat", 
		     "fits_movnam_hdu DIG_CNTS");
  
  // TODO save header data and rows.
}
