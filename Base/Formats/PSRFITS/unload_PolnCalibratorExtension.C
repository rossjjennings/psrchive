#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const PolnCalibratorExtension* pce)
{
  int status = 0;

  if (verbose)
    cerr << "FITSArchive::unload_pce entered" << endl;
  
  // Move to the FEEDPAR Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::pce::unload", 
		     "fits_movnam_hdu FEEDPAR");
  
  // Initialise a new row
  
  fits_delete_rows (fptr, 1, 1, &status);
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::pce::unload", 
		     "fits_insert_rows FEEDPAR");

  char* comment = 0;
  
  // Write NCH_FDPR
  
  int nch_fdpr = pce->get_nchan();
  fits_update_key (fptr, TINT, "NCH_FDPR", &nch_fdpr, comment, &status);

  // Write NCPAR

  int ncpar = 0;
  
  for (int i = 0; i < nch_fdpr; i++) {
    if (pce->get_valid(i))
      ncpar = pce->get_Transformation(i)->get_nparam(); 
  }
  
  fits_update_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);
  
  // Write the channel frequencies
  
  float* dat_freq = new float[nch_fdpr];
  
  for (int i = 0; i < nch_fdpr; i++) {
    dat_freq[i] = 0;
    //dat_freq[i] = pce->get_Transformation(i)->get_freq();
  }
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nch_fdpr, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr,
		  dat_freq, &status);

  delete[] dat_freq;

  if (verbose)
    cerr << "FITSArchive::unload_pce frequencies written" << endl;

  // Write the weights
  
  float* dat_wts = new float[nch_fdpr];

  for (int i = 0; i < nch_fdpr; i++) {
    dat_wts[i] = pce->get_valid(i);
  }
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, nch_fdpr, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, 
		  dat_wts, &status);

  delete[] dat_wts;
  
  if (verbose)
    cerr << "FITSArchive::unload_pce weights written" << endl;
  
  // Write the data itself
  
  long dimension = nch_fdpr * ncpar;
  
  float* data = new float[dimension];
  
  int count = 0;
  for (int i = 0; i < nch_fdpr; i++) {
    for (int j = 0; j < ncpar; j++) {
      data[count] = pce->get_Transformation(i)->get_param(j);
      count++;
    }
  }
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  data, &status);

  delete[] data;
  
  if (verbose)
    cerr << "FITSArchive::unload_pce exiting" << endl; 
}
