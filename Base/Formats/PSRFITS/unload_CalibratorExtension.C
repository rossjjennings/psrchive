/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "CalibratorExtensionIO.h"

using namespace std;

void Pulsar::unload (fitsfile* fptr, const CalibratorExtension* ext)
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::unload CalibratorExtension entered" << endl;
  
  int nchan = ext->get_nchan ();
  int status = 0;
  char* comment = 0;
  
  // Write NCHAN  
  fits_update_key (fptr, TINT, "NCHAN", &nchan, comment, &status);

  // Write EPOCH
  unsigned precision = 4;
  string epoch = ext->get_epoch().printdays (precision);

  char* epoch_str = const_cast<char*>( epoch.c_str() );
  fits_update_key (fptr, TSTRING, "EPOCH", epoch_str, comment, &status);

  long dimension = nchan;
  auto_ptr<float> data ( new float[dimension] );

  // Write the channel frequencies
    
  for (int i = 0; i < nchan; i++)
    data.get()[i] = ext->get_centre_frequency (i);
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nchan, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nchan,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "Pulsar::unload CalibratorExtension", 
		     "fits_write_col DAT_FREQ");

  if (Archive::verbose == 3) cerr << "Pulsar::unload CalibratorExtension"
		 " frequencies written" << endl;

  // Write the weights
  for (int i = 0; i < nchan; i++)
    data.get()[i] = ext->get_weight(i);
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nchan, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nchan, 
		  data.get(), &status);

  if (status)
    throw FITSError (status, "Pulsar::unload CalibratorExtension", 
		     "fits_write_col DAT_WTS");

  if (Archive::verbose == 3) 
    cerr << "Pulsar::unload CalibratorExtension weights written. exit" << endl;
  
}
