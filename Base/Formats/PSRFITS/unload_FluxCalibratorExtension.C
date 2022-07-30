/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "CalibratorExtensionIO.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const FluxCalibratorExtension* fce)
{
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::unload FluxCalibratorExtension" << endl;
  
  // Move to the FLUX_CAL Binary Table
  
  psrfits_init_hdu (fptr, "FLUX_CAL");

  // Write the number of receptors (receiver channels)
  psrfits_update_key (fptr, "NRCVR", (int)fce->get_nreceptor());

  // Write the standard lot (NCHAN, EPOCH, DAT_FREQ, DAT_WTS)
  Pulsar::unload (fptr, fce);

  unsigned nchan = fce->get_nchan();
  unsigned nreceptor = fce->get_nreceptor();

  unsigned dimension = nchan * nreceptor;

  vector< Estimate<double> > temp (dimension);

  try {

    unsigned ichan = 0;
    unsigned ireceptor = 0;

    vector<unsigned> dimensions (2);
    dimensions[0] = nchan;
    dimensions[1] = nreceptor;

    if (verbose > 2)
      cerr << "FITSArchive::unload FluxCalibratorExtension nchan=" << nchan
           << " nreceptor=" << nreceptor << endl;

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_solution(ichan)->get_S_sys (ireceptor);

    unload_Estimates (fptr, temp, "S_SYS", &dimensions);

    if (verbose > 2)
      cerr << "FITSArchive::unload FluxCalibratorExtension S_SYS written" << endl;

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_solution(ichan)->get_S_cal (ireceptor);

    unload_Estimates(fptr, temp, "S_CAL", &dimensions);

    if (verbose > 2)
      cerr << "FITSArchive::unload FluxCalibratorExtension S_CAL written" << endl;

    /*
      2019-Sep-05 Willem van Straten
      Optionally write new SCALE and RATIO parameters produced by fluxcal -g
    */

    if (!fce->has_scale())
    {
      // Remove these extra columns if we are not using them
      psrfits_delete_col(fptr, "SCALE");
      psrfits_delete_col(fptr, "SCALEERR");
      psrfits_delete_col(fptr, "RATIO");
      psrfits_delete_col(fptr, "RATIOERR");
      return;
    }

    if (verbose > 2)
      cerr << "FITSArchive::unload FluxCalibratorExtension has scale" << endl;

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_solution(ichan)->get_scale (ireceptor);

    unload_Estimates (fptr, temp, "SCALE", &dimensions);

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	temp[ichan + nchan*ireceptor] = fce->get_solution(ichan)->get_gain_ratio (ireceptor);

    unload_Estimates (fptr, temp, "RATIO", &dimensions);

  }
  catch (Error& error) {
    throw error += "Pulsar::FITSArchive::unload FluxCalibratorExtension";
  }
}

