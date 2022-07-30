/***************************************************************************
 *
 *   Copyright (C) 2004 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/FluxCalibratorExtension.h"
#include "CalibratorExtensionIO.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::load_FluxCalibratorExtension (fitsfile* fptr)
{
  int status = 0;
 
  if (verbose > 2)
    cerr << "FITSArchive::load_FluxCalibratorExtension entered" << endl;
  
  // Move to the FLUX_CAL HDU
  
  fits_movnam_hdu (fptr, BINARY_TBL, "FLUX_CAL", 0, &status);
  
  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_FluxCalibratorExtension"
	" no FLUX_CAL HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_FluxCalibratorExtension", 
		     "fits_movnam_hdu FLUX_CAL");

  Reference::To<FluxCalibratorExtension> fce = new FluxCalibratorExtension;

  // Get NCH_FLUX (backward compatibility)
  int nch_flux = 0;
  psrfits_read_key (fptr, "NCH_FLUX", &nch_flux, 0, verbose > 2);
  
  if (nch_flux >= 0)
    fce->set_nchan(nch_flux);

  Pulsar::load (fptr, fce);

  if (fce->get_nchan() == 0)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_FluxCalibratorExtension FLUX_CAL HDU"
	   << " contains no data. FluxCalibratorExtension not loaded" << endl;
    return;
  }

  // Get NRCVR
  int nrcvr = 0;
  psrfits_read_key (fptr, "NRCVR", &nrcvr, 1, verbose > 2);
  fce->set_nreceptor (nrcvr);

  unsigned nchan = fce->get_nchan();
  unsigned nreceptor = fce->get_nreceptor();

  unsigned dimension = nchan * nreceptor;

  vector< Estimate<double> > temp (dimension);

  try {

    unsigned ichan = 0;
    unsigned ireceptor = 0;

    if (verbose > 2)
      cerr << "FITSArchive::load_FluxCalibratorExtension loading S_SYS" << endl;

    try {
      // backward compatibility
      load_Estimates (fptr, temp, "T_SYS");
    }
    catch (Error&) {
      load_Estimates (fptr, temp, "S_SYS");
    }

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	fce->get_solution(ichan)->set_S_sys (ireceptor, temp[ichan + nchan*ireceptor]);

    if (verbose > 2)
      cerr << "FITSArchive::load_FluxCalibratorExtension loading S_CAL" << endl;

    try {
      // backward compatibility
      load_Estimates (fptr, temp, "T_CAL");
    }
    catch (Error&) {
      load_Estimates (fptr, temp, "S_CAL");
    }

    for (ichan=0; ichan < nchan; ichan++)
      for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	fce->get_solution(ichan)->set_S_cal (ireceptor, temp[ichan + nchan*ireceptor]);

    /*
      2019-Sep-05 Willem van Straten
      Optionally parse new SCALE and RATIO parameters produced by fluxcal -g
    */

    try {

      if (verbose > 2)
        cerr << "FITSArchive::load_FluxCalibratorExtension"
                " loading SCALE (optional)" << endl;

      load_Estimates (fptr, temp, "SCALE");

      if (verbose > 2)
        cerr << "FITSArchive::load_FluxCalibratorExtension"
                " SCALE loaded" << endl;

      fce->has_scale (true);

      for (ichan=0; ichan < nchan; ichan++)
	for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	  fce->get_solution(ichan)->set_scale (ireceptor, temp[ichan + nchan*ireceptor]);

    }
    catch (Error& error)
    {
      if (verbose > 2)
	cerr << "FITSArchive::load_FluxCalibratorExtension fail SCALE: "
	     << error.get_message();

      fce->has_scale (false);
    }

    try {

      if (verbose > 2)
        cerr << "FITSArchive::load_FluxCalibratorExtension"
                " loading RATIO (optional)" << endl;

      load_Estimates (fptr, temp, "RATIO");

      if (verbose > 2)
        cerr << "FITSArchive::load_FluxCalibratorExtension"
                " RATIO loaded" << endl;

      for (ichan=0; ichan < nchan; ichan++)
	for (ireceptor=0; ireceptor < nreceptor; ireceptor++)
	  fce->get_solution(ichan)->set_gain_ratio (ireceptor, temp[ichan+nchan*ireceptor]);

    }
    catch (Error& error)
    {
      if (verbose > 2)
	cerr << "FITSArchive::load_FluxCalibratorExtension fail RATIO: "
	     << error.get_message();
    }

  }
  catch (Error& error) {
    throw error += "Pulsar::FITSArchive::load_FluxCalibratorExtension";
  }

  add_extension (fce);
  
  if (verbose > 2)
    cerr << "FITSArchive::load_FluxCalibratorExtension exiting" << endl;
}

