#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalibratorStokes.h"
#include "FITSError.h"

#include <assert.h>

#ifdef sun
#include <ieeefp.h>
#endif

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const CalibratorStokes* stokes)
{
  int status = 0;

  if (verbose)
    cerr << "FITSArchive::unload CalibratorStokes entered" << endl;
  
  // Move to the CAL_POLN Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "CAL_POLN", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_movnam_hdu CAL_POLN");
  
  // Initialise a new row
  
  fits_delete_rows (fptr, 1, 1, &status);
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_insert_rows CAL_POLN");

  int nch_poln = stokes->get_nchan();

  if (verbose) cerr << "FITSArchive::unload CalibratorStokes nchan=" 
		    << nch_poln << endl;

  char* comment = 0;

  // Write NCH_POLN  
  fits_update_key (fptr, TINT, "NCH_POLN", &nch_poln, comment, &status);

  long dimension = nch_poln * 3;  
  auto_ptr<float> data ( new float[dimension] );

  // Write the channel frequencies
    
  for (int i = 0; i < nch_poln; i++)
    data.get()[i] = 0;
  

  // Write the weights
  
  for (int i = 0; i < nch_poln; i++)
    data.get()[i] = stokes->get_valid(i);
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nch_poln, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nch_poln, 
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_write_col DAT_WTS");

  if (verbose) cerr << "FITSArchive::unload CalibratorStokes"
		 " weights written" << endl;
  
  // Write the model parameters
    
  int count = 0;
  #ifdef sun
    for (count = 0; count < dimension; count++)
      data.get()[count] = FP_QNAN;
  #else
    for (count = 0; count < dimension; count++)
      data.get()[count] = NAN;
  #endif

  count = 0;
  for (int i = 0; i < nch_poln; i++)
    if (stokes->get_valid(i))
      for (int ipol = 1; ipol < 4; ipol++) {
	data.get()[count] = stokes->get_stokes(i)[ipol].val;
	count++;
      }
    else
      count += 3;

  assert (count == dimension);

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_write_col DATA");

  // Write the variance of the model parameters
    
  count = 0;
  for (int i = 0; i < nch_poln; i++)
    if (stokes->get_valid(i))
      for (int ipol = 1; ipol < 4; ipol++) {
	data.get()[count] = sqrt( stokes->get_stokes(i)[ipol].var );
	count++;
      }
    else
      count += 3;

  assert (count == dimension);

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATAERR", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data.get(), &status);

  if (status)
    throw FITSError (status, "FITSArchive::unload CalibratorStokes", 
		     "fits_write_col DATAERR");
  if (verbose)
    cerr << "FITSArchive::unload CalibratorStokes exiting" << endl; 
}
