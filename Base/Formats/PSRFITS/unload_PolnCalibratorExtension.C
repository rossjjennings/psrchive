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

#include <assert.h>

using namespace std;

void unload_variances (fitsfile*, const Pulsar::PolnCalibratorExtension*,
		       int ncpar, vector<float>& data);

void unload_covariances (fitsfile*, const Pulsar::PolnCalibratorExtension*,
			 int ncovar, vector<float>& data);

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const PolnCalibratorExtension* pce)
try {

  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_movnam_hdu FEEDPAR");
  
  psrfits_clean_rows (fptr);

  // Initialise a new row
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload PolnCalibratorExtension", 
		     "fits_insert_rows FEEDPAR");

  int nchan = pce->get_nchan();
  int ncpar = pce->get_nparam();
  int ncovar = 0;

  if (pce->get_has_covariance())
    ncovar = ncpar * (ncpar+1) / 2;

  if (ncpar == 0)
    throw Error (InvalidState, "FITSArchive::unload PolnCalibratorExtension",
		 "number of model parameters == 0");

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension nchan=" 
	 << nchan <<  " nparam=" << ncpar << " ncovar=" << ncovar << endl;

  string cal_mthd = Calibrator::Type2str( pce->get_type() );

  // Write CAL_MTHD
  psrfits_update_key (fptr, "CAL_MTHD", cal_mthd);

  // Write NCPAR
  psrfits_update_key (fptr, "NCPAR", ncpar);

  // Write NCOVAR
  psrfits_update_key (fptr, "NCOVAR", ncovar);

  Pulsar::unload (fptr, pce);

  long dimension = nchan * ncpar;  
  vector<float> data ( dimension );

  int count = 0;
  for (count = 0; count < dimension; count++)
    data[count] = fits_nullfloat;

  count = 0;
  for (int ichan = 0; ichan < nchan; ichan++) {

    if (pce->get_valid(ichan)) {
      for (int j = 0; j < ncpar; j++) {
	data[count] = pce->get_transformation(ichan)->get_param(j);
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

  psrfits_write_col (fptr, "DATA", data);

  if (ncovar)
    unload_covariances (fptr, pce, ncovar, data);
  else
    unload_variances (fptr, pce, ncpar, data);

  if (verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension exiting" << endl; 

}
 catch (Error& error) {
   throw error += "FITSArchive::unload PolnCalibratorExtension";
 }

void unload_variances (fitsfile* fptr,
		       const Pulsar::PolnCalibratorExtension* pce,
		       int ncpar, vector<float>& data)
{
  unsigned nchan = pce->get_nchan();

  data.resize( ncpar * nchan );

  unsigned count = 0;
  for (int i = 0; i < nchan; i++)
    if (pce->get_valid(i))
      for (int j = 0; j < ncpar; j++) {
	data[count] = sqrt(pce->get_transformation(i)->get_variance(j));
	count++;
      }
    else
      count += ncpar;

  assert (count == data.size());

  psrfits_write_col (fptr, "DATAERR", data);
}

void unload_covariances (fitsfile* fptr,
			 const Pulsar::PolnCalibratorExtension* pce,
			 int ncovar, vector<float>& data)
{
  unsigned nchan = pce->get_nchan();

  if (Pulsar::Archive::verbose == 3)
    cerr << "FITSArchive::unload PolnCalibratorExtension"
      " ncovar = " << ncovar << endl;

  data.resize( ncovar * nchan );

  vector<double> covar;
  unsigned count = 0;

  for (int ichan = 0; ichan < nchan; ichan++) {

    if (!pce->get_valid(ichan)) {
      count += ncovar;
      continue;
    }

    pce->get_transformation(ichan)->get_covariance (covar);

    assert (covar.size() == ncovar);

    for (int j = 0; j < ncovar; j++) {
      data[count] = covar[j];
      count++;
    }

  }

  assert (count == data.size());

  psrfits_write_col (fptr, "COVAR", data);
}
