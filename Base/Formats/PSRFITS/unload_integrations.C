/***************************************************************************
 *
 *   Copyright (C) 2003-2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProfileColumn.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/FourthMoments.h"
#include "Pulsar/CalInfoExtension.h"

#include "FITSError.h"
#include "psrfitsio.h"

#define REPORT_IO_TIMES 0

#if REPORT_IO_TIMES
#include "RealTimer.h"
#endif

using namespace std;

void Pulsar::FITSArchive::unload_integrations (fitsfile* ffptr) const
{
  int status = 0;
  char* comment = 0;

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::unload_integrations" << endl;

  // Move to the SUBINT Binary Table
  
  fits_movnam_hdu (ffptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
                     "fits_movnam_hdu SUBINT");

  char* valid = "VALID";
  fits_update_key (ffptr, TSTRING, "EPOCHS", valid, comment, &status);

  clean_Pointing_columns (ffptr);
  add_Pointing_columns (ffptr);

  const CalInfoExtension* calinfo = get<CalInfoExtension>();
  bool calfreq_set = calinfo && calinfo->cal_frequency > 0.0;

  int perstatus=0;
  int percolnum=0;
  fits_get_colnum(ffptr, CASEINSEN, "PERIOD", &percolnum, &perstatus);

  // Write folding period if predictor model does not exist
  if (perstatus == 0 && (has_model() || calfreq_set))
  {
    if (verbose > 2) 
      cerr << "Pulsar::FITSArchive::unload_integrations delete PERIOD column"
	   << endl;

    fits_delete_col (ffptr, percolnum, &perstatus);
  }
  else if (perstatus)
  {
    if (verbose > 2) 
      cerr << "Pulsar::FITSArchive::unload_integrations insert PERIOD column"
	   << endl;

    perstatus = 0;

    fits_get_colnum(ffptr, CASEINSEN, "OFFS_SUB", &percolnum, &perstatus);
    fits_insert_col (ffptr, percolnum+1, "PERIOD", "1D", &perstatus);

    if (perstatus)
      throw FITSError (perstatus, "Pulsar::FITSArchive::unload_integrations",
                       "fits_insert_col PERIOD");

    psrfits_update_key (ffptr, "TUNIT", percolnum+1, "s",
			"Spin period in seconds");
  }

  // Update the header information
  
  string order_name = "TIME";
  string order_unit = "SEC";

  const IntegrationOrder* order = get<IntegrationOrder>();
  if (order)
  {
    order_name = order->get_extension_name();
    order_unit = order->get_Unit();
  }

  psrfits_update_key (ffptr, "INT_TYPE", order_name);
  psrfits_update_key (ffptr, "INT_UNIT", order_unit);

  /*
    WvS - 07 Feb 2008
    state_scale and state_pol_type strings are set in update_history method
  */
  psrfits_update_key (ffptr, "SCALE", state_scale);
  psrfits_update_key (ffptr, "POL_TYPE", state_pol_type);

  psrfits_update_key (ffptr, "NPOL", (int) get_npol());
  psrfits_update_key (ffptr, "NBIN", (int) get_nbin());
  psrfits_update_key (ffptr, "NCHAN", (int) get_nchan());

  psrfits_update_key (ffptr, "CHAN_BW", get_bandwidth() / get_nchan());
  psrfits_update_key (ffptr, "REFFREQ", get_centre_frequency());
  
  psrfits_update_key (ffptr, "DM", get_dispersion_measure ());
  psrfits_update_key (ffptr, "RM", get_rotation_measure ());

  naux_profile = 0;
  aux_nsample = 0;
  
  const MoreProfiles* more = get_Profile(0,0,0)->get<MoreProfiles>();
  if (more)
  {
    naux_profile = more->get_size ();
    psrfits_update_key (ffptr, "NAUX", (int) naux_profile);

    const FourthMoments* fourth = dynamic_cast<const FourthMoments*> (more);
    if (fourth)
    {
      aux_nsample = fourth->get_nsample();
      // cerr << "FITSArchive::unload_integrations nsample=" << aux_nsample << endl;
      psrfits_update_key (ffptr, "AUXNSAMP", aux_nsample);
    }
  }
  
  // Set the sizes of the columns which may have changed
  int colnum = 0;
  fits_get_colnum (ffptr, CASEINSEN, "DAT_FREQ", &colnum, &status);

#if REPORT_IO_TIMES
  RealTimer clock;
  clock.start();
#endif

  fits_modify_vector_len (ffptr, colnum, nchan, &status);

#if REPORT_IO_TIMES
  clock.stop();
  cerr << "FITSArchive::unload_integrations fits_modify_vector_len"
    " (DAT_FREQ) took " << clock.get_elapsed() << " sec" << endl;
#endif

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
                     "error resizing DAT_FREQ");

  if (verbose > 2)
    cerr << "FITSArchive::unload_integrations DAT_FREQ resized to "
         << nchan << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_WTS", &colnum, &status);

#if REPORT_IO_TIMES
  clock.start();
#endif

  fits_modify_vector_len (ffptr, colnum, nchan, &status);

#if REPORT_IO_TIMES
  clock.stop();
  cerr << "FITSArchive::unload_integrations fits_modify_vector_len"
    " (DAT_WTS) took " << clock.get_elapsed() << " sec" << endl;
#endif

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
                     "error resizing DAT_WTS");

  if (verbose > 2)
    cerr << "FITSArchive::unload_integrations DAT_WTS resized to "
         << nchan << endl;

  setup_dat (ffptr, unload_dat_io);
  unload_dat_io->resize ();

  unsigned nrow = unload_dat_io->get_nrow();
  
  if (verbose > 2)
    cerr << "FITSArchive::unload_integrations dat_io=" << unload_dat_io.ptr()
         << endl;

  if (more)
  {
    setup_aux (ffptr, unload_aux_io, more->get_size());
    unload_aux_io->create (unload_dat_io->get_data_colnum() + 1);
  }

  // Iterate over all rows, calling the unload_integration function to
  // fill in the next spot in the file.
  
  for (unsigned i = 0; i < nsubint; i++)
    unload_Integration (ffptr, i+1, get_Integration(i));

  if (verbose > 2)
    cerr << "FITSArchive::unload_integrations exit" << endl;
}

