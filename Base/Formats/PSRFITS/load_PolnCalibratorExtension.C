#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::load_PolnCalibratorExtension (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status == BAD_HDU_NUM) {
    if (verbose)
      cerr << "Pulsar::FITSArchive::load_PolnCalibratorExtension no FEEDPAR HDU" << endl;

    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_PolnCalibratorExtension", 
		     "fits_movnam_hdu FEEDPAR");

  Reference::To<PolnCalibratorExtension> mypce = new PolnCalibratorExtension;

  char* comment = 0;
  float nullfloat = 0.0;

  // Get NCH_FDPR

  int nch_fdpr = 0;
  fits_read_key (fptr, TINT, "NCH_FDPR", &nch_fdpr, comment, &status);
  
  if (status == 0) {
    mypce->set_nchan(nch_fdpr);
  }

  // Get NCPAR
  
  int ncpar = 0;
  fits_read_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);
  
  // Get CAL_MTHD
  
  char* cal_mthd = new char[80];
  fits_read_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);
  
  /*

    call mypce->set_type() depending on the value of this string

   */

  delete[] cal_mthd;

  // Read the centre frequencies
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  float* dat_freq = new float[nch_fdpr];
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, &nullfloat,
		 dat_freq, &initflag, &status);
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension channel frequencies read" << endl;
  
  for (int i = 0; i < nch_fdpr; i++) {
    //mypce->get_Transformation(i)->set_freq(dat_freq[i]);
  }

  delete[] dat_freq;
  
  // Read the data scale weights
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  float* dat_wts = new float[nch_fdpr];
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, &nullfloat, 
		 dat_wts, &initflag, &status);
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension weights read" << endl;
  
  for (int i = 0; i < nch_fdpr; i++) {
    //mypce->get_Transformation(i)->set_weight(dat_wts[i]);
  }

  delete[] dat_wts;

  // Read the data itself
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  
  long dimension = nch_fdpr * ncpar;
  float* data = new float[dimension];
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &nullfloat, 
		 data, &initflag, &status);
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension data read" << endl;
  
  int count = 0;
  for (int i = 0; i < nch_fdpr; i++) {
    for (int j = 0; j < ncpar; j++) {
      // mypce->get_Transformation(i)->set_param(j,data[count]);
      count++;
    }
  }

  add_extension(mypce);
  
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension exiting" << endl;
}
