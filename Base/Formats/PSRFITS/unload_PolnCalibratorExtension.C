#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "FITSError.h"

#include <assert.h>

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const PolnCalibratorExtension* pce)
{
  int status = 0;

  if (verbose)
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

  int nch_fdpr = pce->get_nchan();
  int ncpar = 0;
  
  for (int i = 0; i < nch_fdpr; i++)
    if (pce->get_valid(i))
      ncpar = pce->get_transformation(i)->get_nparam(); 

  if (verbose) cerr << "FITSArchive::unload PolnCalibratorExtension nchan=" 
		    << nch_fdpr <<  " nparam=" << ncpar << endl;

  char* comment = 0;

  char* cal_mthd= const_cast<char*>( Calibrator::Type2str( pce->get_type() ) );

  // Write CAL_MTHD
  fits_update_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);

  // Write NCH_FDPR  
  fits_update_key (fptr, TINT, "NCH_FDPR", &nch_fdpr, comment, &status);

  // Write NCPAR
  fits_update_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);
  
  // Write EPOCH
  unsigned precision = 4;
  string epoch = pce->get_epoch().printdays (precision);

  char* epoch_str = const_cast<char*>( epoch.c_str() );
  fits_update_key (fptr, TSTRING, "EPOCH", epoch_str, comment, &status);

  long dimension = nch_fdpr * ncpar;  
  auto_ptr<float> data ( new float[dimension] );

  // Write the channel frequencies
    
  for (int i = 0; i < nch_fdpr; i++)
    data.get()[i] = 0;
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nch_fdpr, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_write_col DAT_FREQ");

  if (verbose) cerr << "FITSArchive::unload PolnCalibratorExtension"
		 " frequencies written" << endl;

  // Write the weights
  
  for (int i = 0; i < nch_fdpr; i++)
    data.get()[i] = pce->get_valid(i);
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nch_fdpr, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, 
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_write_col DAT_WTS");

  if (verbose) cerr << "FITSArchive::unload PolnCalibratorExtension"
		 " weights written" << endl;
  
  // Write the model parameters
    
  int count = 0;
  #ifdef sun
    for (count = 0; count < dimension; count++)
      data.get()[count] = 0.0;
  #else
    for (count = 0; count < dimension; count++)
      data.get()[count] = NAN;
  #endif

  count = 0;
  for (int i = 0; i < nch_fdpr; i++)
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	data.get()[count] = pce->get_transformation(i)->get_param(j);
	count++;
      }
    else
      count += ncpar;

  assert (count == dimension);

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_write_col DATA");

  // Write the variance of the model parameters
    
  count = 0;
  for (int i = 0; i < nch_fdpr; i++)
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	data.get()[count] = sqrt( pce->get_transformation(i)->get_variance(j) );
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
  if (verbose)
    cerr << "FITSArchive::unload PolnCalibratorExtension exiting" << endl; 
}
