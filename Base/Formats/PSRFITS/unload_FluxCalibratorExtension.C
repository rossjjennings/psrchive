#include "Pulsar/FITSArchive.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionIO.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const FluxCalibratorExtension* fce)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload FluxCalibratorExtension entered" << endl;
  
  // Move to the FEEDPAR Binary Table
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FEEDPAR", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload FluxCalibratorExtension", 
		     "fits_movnam_hdu FEEDPAR");
  
  // Initialise a new row
  
  fits_delete_rows (fptr, 1, 1, &status);
  
  fits_insert_rows (fptr, 0, 1, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload FluxCalibratorExtension", 
		     "fits_insert_rows FEEDPAR");

  char* comment = 0;

  char* cal_mthd = "";

  // Write CAL_MTHD
  fits_update_key (fptr, TSTRING, "CAL_MTHD", cal_mthd, comment, &status);

  Pulsar::unload (fptr, fce);

  vector< Estimate<double> > temp (fce->get_nchan());
  unsigned ichan = 0;

  try {

    for (ichan=0; ichan < fce->get_nchan(); ichan++)
      temp[ichan] = fce->get_T_sys (ichan);
    unload_Estimates (fptr, temp, "T_SYS");

    for (ichan=0; ichan < fce->get_nchan(); ichan++)
      temp[ichan] = fce->get_cal_flux (ichan);
    unload_Estimates(fptr, temp, "T_CAL");

  }
  catch (Error& error) {
    throw error += "Pulsar::FITSArchive::load_FluxCalibratorExtension";
  }

  if (verbose == 3)
    cerr << "FITSArchive::unload FluxCalibratorExtension exiting" << endl; 
}
