#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalibratorStokes.h"
#include "FITSError.h"

#include <assert.h>

#ifdef sun
#include <ieeefp.h>
#endif

void Pulsar::FITSArchive::load_CalibratorStokes (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose)
    cerr << "FITSArchive::load_CalibratorStokes entered" << endl;
  
  // Move to the CAL_POLN HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "CAL_POLN", 0, &status);
  
  if (status == BAD_HDU_NUM) {
    if (verbose) cerr << "Pulsar::FITSArchive::load_CalibratorStokes"
		   " no CAL_POLN HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_CalibratorStokes", 
		     "fits_movnam_hdu CAL_POLN");

  Reference::To<CalibratorStokes> stokes = new CalibratorStokes;

  char* comment = 0;

  // Get NCH_POLN
  int nch_poln = 0;
  fits_read_key (fptr, TINT, "NCH_POLN", &nch_poln, comment, &status);
  
  if (status == 0)
    stokes->set_nchan(nch_poln);

  long dimension = nch_poln * 3;  
  
  if (dimension == 0) {
    if (verbose)
      cerr << "FITSArchive::load_CalibratorStokes CAL_POLN HDU"
	   << " contains no data. CalibratorStokes not loaded" << endl;
    return;
  }
  
  auto_ptr<float> data ( new float[dimension] );

  #ifdef sun
    float nullfloat = FP_QNAN;
  #else
    float nullfloat = NAN;
  #endif

  // Read the data scale weights
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nch_poln, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load CalibratorStokes", 
		     "fits_read_col DAT_WTS");

  if (verbose)
    cerr << "FITSArchive::load_CalibratorStokes weights read" << endl;
  
  for (int ichan=0; ichan < nch_poln; ichan++) {
    float weight = data.get()[ichan];
    stokes->set_valid (ichan, finite(weight) && weight != 0);
  }

  // Read the data itself
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load CalibratorStokes", 
		     "fits_read_col DATA");

  if (verbose)
    cerr << "FITSArchive::load_CalibratorStokes data read" << endl;
  
  int count = 0;
  for (int ichan = 0; ichan < nch_poln; ichan++) {

    if (!stokes->get_valid(ichan)) {
      count += 3;
      continue;
    }

    Stokes< Estimate<float> > s;

    for (unsigned ipol = 1; ipol < 4; ipol++) {
      s[ipol] = data.get()[count];
      count++;
    }

    stokes->set_stokes (ichan, s);

  }

  assert (count == dimension);

  fits_get_colnum (fptr, CASEINSEN, "DATAERR", &colnum, &status);

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "FITSArchive::load CalibratorStokes", 
		     "fits_read_col DATAERR");

  if (verbose)
    cerr << "FITSArchive::load_CalibratorStokes data error read" << endl;
  
  count = 0;
  for (int ichan = 0; ichan < nch_poln; ichan++) {

    if (!stokes->get_valid(ichan)) {
      count += 3;
      continue;
    }

    Stokes< Estimate<float> > s = stokes->get_stokes (ichan);

    for (unsigned ipol = 1; ipol < 4; ipol++) {
      float err = data.get()[count];
      s[ipol].var = err * err;
      count++;
    }

    stokes->set_stokes (ichan, s);

  }

  assert (count == dimension);

  add_extension (stokes);
  
  if (verbose)
    cerr << "FITSArchive::load_CalibratorStokes exiting" << endl;
}
