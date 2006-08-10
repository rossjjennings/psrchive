/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include "FITSError.h"

void unload (fitsfile* fptr, const Pulsar::DigitiserStatistics::row* drow)
{
  int row = drow->index;

  if (row <= 0)
    throw Error (InvalidParam, "unload (Pulsar::DigitiserStatistics::row*)",
		 "digistat_row invalid row number=%d", row);

  int status = 0;
  char* comment = 0;

  // Write DIG_MODE
  char* tempstr = const_cast<char*>(drow->dig_mode.c_str());
  fits_update_key (fptr, TSTRING, "DIG_MODE", tempstr, comment, &status);
  
  // Write NDIGR
  
  fits_update_key (fptr, TINT, "NDIGR", 
		   (int*)&(drow->ndigr), comment, &status);
  
  // Write NPAR
  
  fits_update_key (fptr, TINT, "NPAR", 
		   (int*)&(drow->nlev), comment, &status);
  
  // Write NCYCSUB
  
  fits_update_key (fptr, TINT, "NCYCSUB", 
		   (int*)&(drow->ncycsub), comment, &status);
  
  // Write DIGLEV
  tempstr = const_cast<char*>(drow->diglev.c_str());
  fits_update_key (fptr, TSTRING, "DIGLEV", tempstr, comment, &status);
  
  // Write the data itself
  if (status)
    throw FITSError (status, "unload (Pulsar::DigitiserStatistics::row*)",
		     "fit_update_key(s)");

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status); 
  fits_modify_vector_len (fptr, colnum, drow->data.size(), 
			  &status);
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 
		  drow->data.size(), 
		  (float*)&(drow->data[0]), &status);

  if (status)
    throw FITSError (status, "unload (Pulsar::DigitiserStatistics::row*)",
		     "fit_write_col DATA");
  
}

void 
Pulsar::FITSArchive::unload (fitsfile* fptr, const DigitiserStatistics* dstats)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_digistat entered" << endl;

  // Move to the DIG_STAT HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat", 
		     "fits_movnam_hdu DIG_STAT");
  
  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat", 
                     "fits_get_num_rows DIG_STAT");
  
  fits_delete_rows (fptr, 1, numrows, &status);
  
  fits_insert_rows (fptr, 0, (dstats->rows).size(), &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_digistat",
                     "fits_insert_rows DIG_STAT");
  
  try {
    
    for (unsigned i = 0; i < (dstats->rows).size(); i++)
      ::unload( fptr, &(dstats->rows[i]) );

  }
  catch (Error& error) {
    throw error += "FITSArchive::unload_digistat";
  }

  if (verbose == 3)
    cerr << "FITSArchive::unload_digistat exiting" << endl;
}
