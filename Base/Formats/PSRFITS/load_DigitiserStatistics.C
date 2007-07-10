/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserStatistics.h"
#include "psrfitsio.h"
#include <tostring.h>

using namespace std;

void load (fitsfile* fptr, Pulsar::DigitiserStatistics::row* drow)
{
  int row = drow->index;

  if (row <= 0)
    throw Error (InvalidParam, "load (Pulsar::DigitiserStatistics::row*)",
                 "invalid row number=%d", row);

  if (Pulsar::Archive::verbose > 2)
  {
    cerr << "FITSArchive::load_digistat_row entered" << endl;
    cerr << "loading row #" << row << endl;
  }

  // Get DIG_MODE

  psrfits_read_key (fptr, "DIG_MODE", &(drow->dig_mode));

  if (Pulsar::Archive::verbose > 2)
    cerr << "Read DIG_MODE = " << drow->dig_mode << endl;

  // Get NDIGR

  psrfits_read_key (fptr, "NDIGR", &(drow->ndigr));


  // Get NPAR (called NLEV prior to version 2.3)

  psrfits_read_key (fptr, "NLEV", &(drow->nlev), 0,
                    Pulsar::Archive::verbose > 2);

  if (!(drow->nlev))
    psrfits_read_key (fptr, "NPAR", &(drow->nlev));

  // Get NCYCSUB

  psrfits_read_key (fptr, "NCYCSUB", &(drow->ncycsub));

  // Get DIGLEV

  psrfits_read_key (fptr, "DIGLEV", &(drow->diglev));

  // Read the data itself

  drow->data.resize( drow->nlev * drow->ndigr * drow->ncycsub );

  float nullfloat = 0.0;
  psrfits_read_col (fptr, "DATA", drow->data, row, nullfloat);

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::load_digistat_row exiting" << endl;
}

void Pulsar::FITSArchive::load_DigitiserStatistics (fitsfile* fptr)
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_digistat entered" << endl;

  // Move to the DIG_STAT HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_STAT", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
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

  string s_data;


  // load the DIGLEV from HDU
  psrfits_read_key( fptr, "DIGLEV", &s_data );
  dstats->set_diglev( s_data );

  // load the NPAR from HDU
  try
  {
    psrfits_read_key( fptr, "NPAR", &s_data );
  }
  catch( Error e )
  {
    psrfits_read_key( fptr, "NLEV", &s_data );
  }
  dstats->set_npar( fromstring<unsigned int>(s_data) );
  
  // load the NCYCSUB from the HDU
  psrfits_read_key( fptr, "NCYCSUB", &s_data );
  dstats->set_ncycsub( fromstring<unsigned int>(s_data) );

  // load the NDIGR from HDU
  psrfits_read_key( fptr, "NDIGR", &s_data );
  dstats->set_ndigr( fromstring<unsigned int>(s_data) );


  (dstats->rows).resize(numrows);

  for (int i = 0; i < numrows; i++)
  {
    dstats->rows[i] = DigitiserStatistics::row();
    dstats->rows[i].index = i+1;
    ::load( fptr, &(dstats->rows[i]) );
  }

  add_extension (dstats);

  if (verbose > 2)
    cerr << "FITSArchive::load_digistat exiting" << endl;
}
