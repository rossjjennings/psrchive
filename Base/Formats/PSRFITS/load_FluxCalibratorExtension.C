#include "Pulsar/FITSArchive.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionIO.h"

#include <stdlib.h>
#include <assert.h>

void Pulsar::FITSArchive::load_FluxCalibratorExtension (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose == 3)
    cerr << "FITSArchive::load_FluxCalibratorExtension entered" << endl;
  
  // Move to the FLUX_CAL HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FLUX_CAL", 0, &status);
  
  if (status == BAD_HDU_NUM) {
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_FluxCalibratorExtension"
	" no FLUX_CAL HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_FluxCalibratorExtension", 
		     "fits_movnam_hdu FLUX_CAL");

  Reference::To<FluxCalibratorExtension> fce = new FluxCalibratorExtension;

  char* comment = 0;

  // Get CAL_MTHD
  
  char* cal_mthd = new char[80];

  fits_read_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);  
  if (!status && verbose == 3)
    cerr << "FITSArchive::load_FluxCalibratorExtension method=" 
	 << cal_mthd << endl;

  fits_read_key (fptr, TSTRING, "SCALFILE", cal_mthd, comment, &status);  
  if (!status && verbose == 3)
    cerr << "FITSArchive::load_FluxCalibratorExtension scalfile=" 
	 << cal_mthd << endl;

  delete[] cal_mthd;

  status = 0;

  // Get NCH_FLUX
  int nch_flux = 0;
  fits_read_key (fptr, TINT, "NCH_FLUX", &nch_flux, comment, &status);
  
  if (status == 0 && nch_flux >= 0)
    fce->set_nchan(nch_flux);

  Pulsar::load (fptr, fce);

  if (fce->get_nchan()) {
    if (verbose == 3)
      cerr << "FITSArchive::load_FluxCalibratorExtension FLUX_CAL HDU"
	   << " contains no data. FluxCalibratorExtension not loaded" << endl;
      return;
  }

  vector< Estimate<double> > temp (fce->get_nchan());
  unsigned ichan = 0;

  load_Estimate_vector (fptr, temp, "T_SYS", 
			"Pulsar::FITSArchive::load_FluxCalibratorExtension");
  for (ichan=0; ichan < fce->get_nchan(); ichan++)
    fce->set_T_sys (ichan, temp[ichan]);


  add_extension (fce);
  
  if (verbose == 3)
    cerr << "FITSArchive::load_FluxCalibratorExtension exiting" << endl;
}
