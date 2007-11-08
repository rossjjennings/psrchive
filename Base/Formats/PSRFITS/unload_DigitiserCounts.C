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
using namespace Pulsar;




void unload_counts_table ( fitsfile *fptr, const DigitiserCounts *const_ext )
{
  DigitiserCounts *ext = const_cast<DigitiserCounts*>(const_ext);

  for( int i = 0; i < ext->rows.size(); i ++ )
  {

    if( !ext )
    {
      cerr << "Failed to fetch digitiser counts ext" << endl;
      return;
    }

    try
    {
      psrfits_write_col (fptr, "DAT_SCL", ext->rows[i].data_scl, i+1 );
      psrfits_write_col (fptr, "DAT_OFFS", ext->rows[i].data_offs, i+1 );

      psrfits_write_col (fptr, "DATA", ext->rows[i].data, i+1 );
    }
    catch( Error e )
    {
      cerr << e << endl;
    }
  }
}

void unload_counts_keys( fitsfile *fptr, const DigitiserCounts *ext )
{
  int status = 0;
  int ndigr = ext->get_ndigr();
  string diglev = ext->get_diglev();
  string dig_mode = ext->get_dig_mode();
  int npthist = ext->get_npthist();
  int nlev = ext->get_nlev();
  float dyn_levt = ext->get_dyn_levt();

  cerr << "saving dyn_levt" << dyn_levt << endl;
  cerr << "saving ndigr " << ndigr << endl;
  cerr << "saving diglev " << diglev << endl;
  cerr << "saving dig_mode " << dig_mode << endl;
  cerr << "saving npthist " << npthist << endl;
  cerr << "saving nlev " << nlev << endl;

  char *nullstring = NULL;

  psrfits_update_key( fptr, "DYN_LEVT", dyn_levt );
  psrfits_update_key( fptr, "NDIGR", ndigr );
  psrfits_update_key( fptr, "DIGLEV", diglev );
  psrfits_update_key( fptr, "DIG_MODE", dig_mode );
  psrfits_update_key( fptr, "NPTHIST", npthist );
  psrfits_update_key( fptr, "NLEV", nlev );

  cerr << "digitiser counts keys unloaded" << endl;
}


void
Pulsar::FITSArchive::unload (fitsfile* fptr, const DigitiserCounts* ext )
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload(DigitiserCounts) entered" << endl;

  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_CNTS", 0, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload(DigitiserCounts)",
                     "fits_movnam_hdu DIG_CNTS");

  unload_counts_keys( fptr, ext );

  fits_insert_rows (fptr, 0, ext->rows.size(), &status);

  unload_counts_table( fptr, ext );
}
