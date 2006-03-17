/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "CalibratorExtensionIO.h"

#include <assert.h>

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const PolnCalibratorExtension* pce)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_movnam_hdu FEEDPAR");
  
  // Initialise a new row
  
  fits_delete_rows (fptr, 1, 1, &status);
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_insert_rows FEEDPAR");

  int nchan = pce->get_nchan();
  int ncpar = 0;
  
  for (int i = 0; i < nchan; i++)
    if (pce->get_valid(i))
      ncpar = pce->get_transformation(i)->get_nparam(); 

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension nchan=" 
	 << nchan <<  " nparam=" << ncpar << endl;

  char* comment = 0;

  char* cal_mthd= const_cast<char*>( Calibrator::Type2str( pce->get_type() ) );

  // Write CAL_MTHD
  fits_update_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);

  // Write NCPAR
  fits_update_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);

  Pulsar::unload (fptr, pce);

  long dimension = nchan * ncpar;  
  auto_ptr<float> data ( new float[dimension] );

  int count = 0;
  for (count = 0; count < dimension; count++)
    data.get()[count] = fits_nullfloat;

  count = 0;
  for (int ichan = 0; ichan < nchan; ichan++) {

    if (pce->get_valid(ichan)) {
      for (int j = 0; j < ncpar; j++) {
	data.get()[count] = pce->get_transformation(ichan)->get_param(j);
	count++;
      }
    }
    else  {
      if (verbose == 3)
        cerr << "FITSArchive::unload PolnCalibratorExtension ichan="
             << ichan << " flagged invalid" << endl;
      count += ncpar;
    }

  }

  assert (count == dimension);

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_write_col DATA");

  // Write the variance of the model parameters
    
  count = 0;
  for (int i = 0; i < nchan; i++)
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	data.get()[count] = sqrt(pce->get_transformation(i)->get_variance(j));
	count++;
      }
    else
      count += ncpar;

  assert (count == dimension);

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATAERR", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_write_col DATAERR");
  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension exiting" << endl; 
}
