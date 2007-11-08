/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include "psrfitsio.h"



using namespace std;
using namespace Pulsar;



void unload (fitsfile* fptr, const Pulsar::DigitiserStatistics::row* const_drow)
{
  DigitiserStatistics::row *drow = const_cast<DigitiserStatistics::row*>( const_drow );
  
  int row = drow->index;

  if (row <= 0)
    throw Error (InvalidParam, "unload (Pulsar::DigitiserStatistics::row*)",
		 "digistat_row invalid row number=%d", row);

  int status = 0;
  
  // Write the data itself
  if (status)
    throw FITSError (status, "unload (Pulsar::DigitiserStatistics::row*)",
		     "fit_update_key(s)");
  
  
  psrfits_write_col( fptr, "ATTEN", drow->atten, row );
  
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
  
  
  char* comment = 0;
  int idata = 0;
  
  // Write DIG_MODE
  char* tempstr = const_cast<char*>(dstats->get_dig_mode().c_str());
  fits_update_key (fptr, TSTRING, "DIG_MODE", tempstr, comment, &status);
  
  // Write NDIGR
  idata = dstats->get_ndigr();
  fits_update_key (fptr, TINT, "NDIGR", &(idata), comment, &status);
  
  // Write NPAR
  idata = dstats->get_npar();  
  fits_update_key (fptr, TINT, "NPAR", &(idata), comment, &status);
  
  // Write NCYCSUB
  idata = dstats->get_ncycsub();
  fits_update_key (fptr, TINT, "NCYCSUB", &(idata), comment, &status);
  
  // Write DIGLEV
  tempstr = const_cast<char*>(dstats->get_diglev().c_str());
  fits_update_key (fptr, TSTRING, "DIGLEV", tempstr, comment, &status);

  
  
  
  
  psrfits_clean_rows (fptr);

  // Insert some new rows

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
