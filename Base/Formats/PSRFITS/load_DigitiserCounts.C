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
  int status = 0;

  float nullfloat = 0.0;

  if (verbose > 2)
    cerr << "FITSArchive::load_DigitiserCounts entered" << endl;

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
  float f_data;

  psrfits_read_key( fptr, "DYN_LEVT", &s_data );
  ext->set_dyn_levt( fromstring<float>( s_data ) );

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

  psrfits_read_key( fptr, "NDIGR", &s_data );
  ext->set_ndigr( fromstring<int>( s_data ) );

  // load the rows of data
  int num_rows;
  psrfits_read_key( fptr, "NAXIS2", &s_data );
  num_rows = fromstring<int>( s_data );


  ext->rows.resize( num_rows );

  int data_length = ext->get_npthist() * ext->get_ndigr();

  for( int i = 0; i < num_rows; i ++ )
  {
    psrfits_read_col( fptr, "DAT_OFFS", &(ext->rows[i].data_offs), i+1, 0.0f );
    psrfits_read_col( fptr, "DAT_SCL", &(ext->rows[i].data_scl), i+1, 0.0f );

    int ndigr = ext->get_ndigr();
    int npthist = ext->get_npthist();
    int row_length = ndigr * npthist;

//     vector<int> new_row( row_length );
    ext->rows[i].data.resize( ext->get_ndigr() * ext->get_npthist() );

    psrfits_read_col( fptr, "DATA", ext->rows[i].data, i+1, 0 );

//     for( int d = 0; d < data_length; d ++ )
//     {
//       
//       ext->rows[i].data[d] = float(new_row[d]) * ext->rows[i].data_scl + ext->rows[i].data_offs;
//       if( new_row[d] != int( (ext->rows[d].data[d] - ext->rows[d].data_offs ) / ext->rows[i].data_scl ) )
// 	cerr << "comparison failed at load time" << endl;
//     }
  }

  add_extension( ext );
}






