#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "FITSError.h"

#include <assert.h>

#ifdef sun
#include <ieeefp.h>
#endif

void Pulsar::FITSArchive::load_PolnCalibratorExtension (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status == BAD_HDU_NUM) {
    if (verbose) cerr << "Pulsar::FITSArchive::load_PolnCalibratorExtension"
		   " no FEEDPAR HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_PolnCalibratorExtension", 
		     "fits_movnam_hdu FEEDPAR");

  Reference::To<PolnCalibratorExtension> pce = new PolnCalibratorExtension;

  char* comment = 0;

  // Get CAL_MTHD
  
  char* cal_mthd = new char[80];
  fits_read_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);  
  pce->set_type( Calibrator::str2Type (cal_mthd) );
  delete[] cal_mthd;

  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension Calibrator type=" 
	 << Calibrator::Type2str (pce->get_type()) << endl;

  // Get NCPAR 
  int ncpar = 0;
  fits_read_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);
  
  // Get NCH_FDPR
  int nch_fdpr = 0;
  fits_read_key (fptr, TINT, "NCH_FDPR", &nch_fdpr, comment, &status);
  
  if (status == 0)
    pce->set_nchan(nch_fdpr);

  // Get EPOCH
  char* epoch = new char[80];
  fits_read_key (fptr, TSTRING, "EPOCH", epoch, comment, &status);

  if (status == 0) {
    MJD mjd (epoch);
    pce->set_epoch (mjd);
  }

  delete [] epoch;
  status = 0;

  long dimension = nch_fdpr * ncpar;  
  auto_ptr<float> data ( new float[dimension] );
  
  if (dimension == 0) {
    if (verbose) {
      cerr << "FITSArchive::load_PolnCalibratorExtension FEEDPAR HDU"
	   << " contains no data. PolnCalibratorExtension not loaded" << endl;
    }
    return;
  }
  
  // Read the centre frequencies
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
 
  #ifdef sun
    float nullfloat = 0.0;
  #else
    float nullfloat = NAN;
  #endif
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, &nullfloat,
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DAT_FREQ");

  if (verbose) cerr << "FITSArchive::load_PolnCalibratorExtension"
		 " channel frequencies read" << endl;
  
  // Read the data scale weights
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nch_fdpr, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DAT_WTS");

  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension weights read" << endl;
  
  for (int ichan=0; ichan < nch_fdpr; ichan++)
    if ( !finite(data.get()[ichan]) || data.get()[ichan]==0 )
      pce->set_valid (ichan, false);

  // Read the data itself
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DATA");

  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension data read" << endl;
  
  int count = 0;
  for (int i = 0; i < nch_fdpr; i++) {
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	pce->get_transformation(i)->set_param(j,data.get()[count]);
	count++;
      }
    else
      count += ncpar;
  }

  assert (count == dimension);

  fits_get_colnum (fptr, CASEINSEN, "DATAERR", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DATAERR");

  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension dataerr read" << endl;
  
  count = 0;
  for (int i = 0; i < nch_fdpr; i++) {
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	float err = data.get()[count];
	pce->get_transformation(i)->set_variance (j, err*err);
	count++;
      }
    else
      count += ncpar;
  }

  assert (count == dimension);

  add_extension (pce);
  
  if (verbose)
    cerr << "FITSArchive::load_PolnCalibratorExtension exiting" << endl;
}
