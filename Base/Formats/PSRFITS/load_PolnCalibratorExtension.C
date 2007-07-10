/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "CalibratorExtensionIO.h"
#include "psrfitsio.h"

#include <stdlib.h>
#include <assert.h>

using namespace std;

void load_variances (fitsfile* fptr, Pulsar::PolnCalibratorExtension* pce,
		     int ncpar, vector<float>& data);

void load_covariances (fitsfile* fptr, Pulsar::PolnCalibratorExtension* pce,
		       int ncovar, vector<float>& data);

void Pulsar::FITSArchive::load_PolnCalibratorExtension (fitsfile* fptr)
try {

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
  psrfits_read_key (fptr, "NCPAR", &ncpar, 0, verbose == 3);
  if (ncpar < 0)
    ncpar = 0;
  pce->set_ncpar( ncpar );
//   
//   // Get NCHAN
//   int nchan = 0;
//   psrfits_read_key( fptr, "NCHAN", &nchan, 0, verbose == 3 );
//   if( nchan < 0 )
//     nchan = 0;
//   pce->set_nchan( nchan );
//   
//   // Get EPOCH
//   double epoch;
//   psrfits_read_key( fptr, "EPOCH", &epoch, 0.0, verbose == 3 );
//   pce->set_epoch( epoch );

  int ncovar = 0;
  psrfits_read_key (fptr, "NCOVAR", &ncovar, 0, verbose == 3);

  // Get NCH_FDPR (old versions of PSRFITS header)
  int nch_fdpr = 0;
  psrfits_read_key (fptr, "NCH_FDPR", &nch_fdpr, 0, verbose == 3);
  
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

  vector<float> data (dimension);
  
  psrfits_read_col (fptr, "DATA", data);

  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension data read" << endl;
  
  int count = 0;
  for (ichan = 0; ichan < pce->get_nchan(); ichan++)
    if (pce->get_valid(ichan)) {
      bool valid = true;
      for (int j = 0; j < ncpar; j++) {
	if (!finite(data[count]))
	  valid = false;
	else
	  pce->get_transformation(ichan)->set_param(j,data[count]);
	count++;
      }
      if (!valid)
	pce->set_valid (ichan, false);
    }
    else
      count += ncpar;

  assert (count == dimension);

  if (ncovar)
    load_covariances (fptr, pce, ncovar, data);
  else
    load_variances (fptr, pce, ncpar, data);

  add_extension (pce);
  
  if (verbose == 3)
    cerr << "FITSArchive::load_PolnCalibratorExtension exiting" << endl;

}
 catch (Error& error) {
   throw error += "FITSArchive::load PolnCalibratorExtension";
 }

void load_variances (fitsfile* fptr, Pulsar::PolnCalibratorExtension* pce,
		     int ncpar, vector<float>& data)
{
  data.resize( ncpar * pce->get_nchan() );

  psrfits_read_col (fptr, "DATAERR", data);

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::load_PolnCalibratorExtension dataerr read" << endl;
  
  int count = 0;

  for (unsigned ichan = 0; ichan < pce->get_nchan(); ichan++) {
    if (pce->get_valid(ichan)) {

      bool valid = true;
      int zeroes = 0;

      for (int j = 0; j < ncpar; j++) {

	float err = data[count];

	if (!finite(err))
	  valid = false;
	else
	  pce->get_transformation(ichan)->set_variance (j,err*err);

	if (err == 0)
	  zeroes++;

	count++;	
      }

      if (zeroes == ncpar) {
	if (Pulsar::Archive::verbose > 1)
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

  assert (count == data.size());
}

void load_covariances (fitsfile* fptr, Pulsar::PolnCalibratorExtension* pce,
		       int ncovar, vector<float>& data)
{
  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::load_PolnCalibratorExtension"
      " ncovar=" << ncovar << endl;

  unsigned nchan = pce->get_nchan();

  data.resize( ncovar * nchan );

  psrfits_read_col (fptr, "COVAR", data);

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::load_PolnCalibratorExtension COVAR read" << endl;

  vector<double> covar (ncovar);
  unsigned count = 0;

  for (int ichan = 0; ichan < nchan; ichan++) {

    if (!pce->get_valid(ichan)) {
      count += ncovar;
      continue;
    }

    for (int j = 0; j < ncovar; j++) {
      covar[j] = data[count];
      count++;
    }

    pce->get_transformation(ichan)->set_covariance (covar);

  }

  assert (count == data.size());

  pce->set_has_covariance( true );

}
