/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "CalibratorExtensionIO.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

void Pulsar::FITSArchive::load_PolnCalibratorExtension (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status == BAD_HDU_NUM) {
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_PolnCalibratorExtension"
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

  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension Calibrator type=" 
	 << Calibrator::Type2str (pce->get_type()) << endl;

  // Get NCPAR 
  int ncpar = 0;
  fits_read_key (fptr, TINT, "NCPAR", &ncpar, comment, &status);
  if (ncpar < 0)
    ncpar = 0;

  // Get NCH_FDPR (old versions of PSRFITS header)
  int nch_fdpr = 0;
  fits_read_key (fptr, TINT, "NCH_FDPR", &nch_fdpr, comment, &status);
  
  if (status == 0 && nch_fdpr >= 0)
    pce->set_nchan(nch_fdpr);

  status = 0;

  Pulsar::load (fptr, pce);

  long dimension = pce->get_nchan() * ncpar;  
  
  if (dimension == 0) {
    if (verbose == 3)
      cerr << "FITSArchive::load_PolnCalibratorExtension FEEDPAR HDU"
	   << " contains no data. PolnCalibratorExtension not loaded" << endl;
      return;
  }

  unsigned ichan;

  for (ichan=0; ichan < pce->get_nchan(); ichan++)
    if ( pce->get_weight (ichan) == 0 )  {
      if (verbose == 3)
        cerr << "FITSArchive::load_PolnCalibratorExtension ichan=" << ichan
             << " flagged invalid" << endl;
      pce->set_valid (ichan, false);
    }

  auto_ptr<float> data ( new float[dimension] );
  
  // Read the data  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &fits_nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DATA");

  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension data read" << endl;
  
  int count = 0;
  for (ichan = 0; ichan < pce->get_nchan(); ichan++)
    if (pce->get_valid(ichan)) {
      bool valid = true;
      for (int j = 0; j < ncpar; j++) {
	if (!finite(data.get()[count]))
	  valid = false;
	else
	  pce->get_transformation(ichan)->set_param(j,data.get()[count]);
	count++;
      }
      if (!valid)
	pce->set_valid (ichan, false);
    }
    else
      count += ncpar;


  assert (count == dimension);

  fits_get_colnum (fptr, CASEINSEN, "DATAERR", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &fits_nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load PolnCalibratorExtension", 
		     "fits_read_col DATAERR");

  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension dataerr read" << endl;
  
  count = 0;
  for (ichan = 0; ichan < pce->get_nchan(); ichan++) {
    if (pce->get_valid(ichan)) {

      bool valid = true;
      int zeroes = 0;

      for (int j = 0; j < ncpar; j++) {

	float err = data.get()[count];

	if (!finite(err))
	  valid = false;
	else
	  pce->get_transformation(ichan)->set_variance (j,err*err);

	if (err == 0)
	  zeroes++;

	count++;	
      }

      if (zeroes == ncpar) {
	if (verbose > 1)
	  cerr << "Pulsar::FITSArchive::load_PolnCalibratorExtension WARNING\n"
	    "  ichan=" << ichan << " flagged invalid: "
	    "zero error in all parameters" << endl;
	valid = false;
      }

      if (!valid)
	pce->set_valid (ichan, false);

    }
    else
      count += ncpar;
  }

  assert (count == dimension);

  add_extension (pce);
  
  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension exiting" << endl;
}
