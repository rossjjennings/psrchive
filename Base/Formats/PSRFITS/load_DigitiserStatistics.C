#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include "FITSError.h"

void load (fitsfile* fptr, Pulsar::DigitiserStatistics::row* drow)
{
  int status = 0;
  int row = drow->index;

  if (row <= 0)
    throw Error (InvalidParam, "load (Pulsar::DigitiserStatistics::row*)",
		 "invalid row number=%d", row);
  
  if (Pulsar::Archive::verbose) {
    cerr << "FITSArchive::load_digistat_row entered" << endl;
    cerr << "loading row #" << row << endl;
  }
  
  // Get DIG_MODE
  
  int colnum = 0;
  int initflag = 0;
  float nullfloat = 0.0;
  char* comment = 0;
  
  char* temp = new char[128];
  
  fits_read_key (fptr, TSTRING, "DIG_MODE", temp, comment, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read DIG_MODE = " << temp << endl;
  drow->dig_mode = temp;

  // Get NDIGR
  
  fits_read_key (fptr, TINT, "NDIGR", 
		 &(drow->ndigr), comment, &status);
  
  // Get NLEV
  
  fits_read_key (fptr, TINT, "NLEV", 
		 &(drow->nlev), comment, &status);
  
  // Get NCYCSUB
  
  fits_read_key (fptr, TINT, "NCYCSUB", 
		 &(drow->ncycsub), comment, &status);
  
  // Get DIGLEV

  fits_read_key (fptr, TSTRING, "DIGLEV", temp, comment, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read DIGLEV = " << temp << endl;
  drow->diglev = temp;
  
  // Read the data itself
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  
  int dimension = drow->nlev * drow->ndigr * drow->ncycsub;
  
  drow->data.resize(dimension);
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, dimension, &nullfloat, 
		 &(drow->data[0]), &initflag, &status);

  delete[] temp;

  if (Pulsar::Archive::verbose)
    cerr << "FITSArchive::load_digistat_row exiting" << endl;
}

void Pulsar::FITSArchive::load_DigitiserStatistics (fitsfile* fptr)
{
  int status = 0;
  
  if (verbose)
    cerr << "FITSArchive::load_digistat entered" << endl;

  // Move to the DIG_STAT HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);

  if (status == BAD_HDU_NUM) {
    if (verbose)
      cerr << "Pulsar::FITSArchive::load_digistat no DIG_STAT HDU" << endl;

    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_digistat", 
		     "fits_movnam_hdu DIG_STAT");
  
  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_digistat", 
                     "fits_get_num_rows DIG_STAT");


  Reference::To<DigitiserStatistics> dstats = new DigitiserStatistics;

  (dstats->rows).resize(numrows);
  
  for (int i = 0; i < numrows; i++) {
    dstats->rows[i] = DigitiserStatistics::row();
    dstats->rows[i].index = i+1;
    ::load( fptr, &(dstats->rows[i]) );
  }
  
  add_extension (dstats);

  if (verbose)
    cerr << "FITSArchive::load_digistat exiting" << endl;
}
