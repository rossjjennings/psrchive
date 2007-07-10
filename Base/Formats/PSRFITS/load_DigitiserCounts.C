/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/DigitiserCounts.h"
#include "psrfitsio.h"
#include <tostring.h>
#include <Error.h>

using namespace std;


void Pulsar::FITSArchive::load_DigitiserCounts (fitsfile* fptr)
{
  int status;

  if (verbose > 2)
    cerr << "FITSArchive::load_digistat entered" << endl;

  // Move to the DIG_STAT HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "DIG_CNTS", 0, &status);

  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_DigitiserCounts no DIG_CNTS HDU" << endl;

    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_DigitiserCounts",
                     "fits_movnam_hdu DIG_CNTS");


  Reference::To< DigitiserCounts > ext = new DigitiserCounts();

  if( !ext )
    throw( Error( BadAllocation, "Pulsar::FITSArchive::load_DigitiserCounts", "failed to allocate DigitiserCounts" ) );

  string s_data;

  psrfits_read_key( fptr, "DIG_MODE", &s_data );
  ext->set_dig_mode( s_data );

  psrfits_read_key( fptr, "NLEV", &s_data );
  if( s_data == "*" )
    ext->set_nlev( 0 );
  else
    ext->set_nlev( fromstring<unsigned int>( s_data ) );

  psrfits_read_key( fptr, "NPTHIST", &s_data );
  if( s_data == "*" )
    ext->set_npthist( 0 );
  else
    ext->set_npthist( fromstring<unsigned int>( s_data ) );

  psrfits_read_key( fptr, "DIGLEV", &s_data );
  ext->set_diglev( s_data );

  add_extension( ext );
}






