/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "psrfitsio.h"

using namespace std;

void unload (fitsfile* fptr, const Pulsar::ProcHistory::row* hrow) try
{
  if (!hrow)
    return;

  int row = hrow->index;

  if (Pulsar::Archive::verbose == 3)
    cerr << "unload Pulsar::ProcHistory::row row=" << row << endl;

  psrfits_write_col (fptr, "DATE_PRO", hrow->date_pro, row);
  psrfits_write_col (fptr, "PROC_CMD", hrow->proc_cmd, row);
  psrfits_write_col (fptr, "POL_TYPE", hrow->pol_type, row);
  psrfits_write_col (fptr, "NSUB",     hrow->nsub,     row);
  psrfits_write_col (fptr, "NPOL",     hrow->npol,     row);
  psrfits_write_col (fptr, "NBIN",     hrow->nbin,     row);
  psrfits_write_col (fptr, "NBIN_PRD", hrow->nbin_prd, row);
  psrfits_write_col (fptr, "TBIN",     hrow->tbin,     row);
  psrfits_write_col (fptr, "CTR_FREQ", hrow->ctr_freq, row);
  psrfits_write_col (fptr, "NCHAN",    hrow->nchan,    row);
  psrfits_write_col (fptr, "CHAN_BW",  hrow->chan_bw,  row);
  psrfits_write_col (fptr, "PAR_CORR", hrow->par_corr, row);
  psrfits_write_col (fptr, "FA_CORR",  hrow->fa_corr,  row);
  psrfits_write_col (fptr, "RM_CORR",  hrow->rm_corr,  row);
  psrfits_write_col (fptr, "DEDISP",   hrow->dedisp,   row);
  psrfits_write_col (fptr, "DDS_MTHD", hrow->dds_mthd, row);
  psrfits_write_col (fptr, "SC_MTHD",  hrow->sc_mthd,  row);
  psrfits_write_col (fptr, "CAL_MTHD", hrow->cal_mthd, row);
  psrfits_write_col (fptr, "CAL_FILE", hrow->cal_file, row);
  psrfits_write_col (fptr, "RFI_MTHD", hrow->rfi_mthd, row);
  psrfits_write_col (fptr, "IFR_MTHD", hrow->ifr_mthd, row);
  psrfits_write_col (fptr, "SCALE",    hrow->scale,    row);

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::unload_hist_row exiting" << endl;
}
catch (Error& error)
{
  throw error += "unload Pulsar::ProcHistory::row";
}

void Pulsar::FITSArchive::unload (fitsfile* fptr, const ProcHistory* history)
try
{
  unsigned numrows = history->rows.size();

  if (verbose == 3)
    cerr << "FITSArchive::unload ProcHistory rows=" << numrows << endl;

  // Move to the HISTORY HDU

  int status = 0;

  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_movnam_hdu HISTORY");
  
  psrfits_clean_rows (fptr);

  fits_insert_rows (fptr, 0, numrows, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_insert_rows HISTORY");
  
  for (unsigned i = 0; i < numrows; i++) 
    ::unload (fptr, &(history->rows[i]));
  
  if (verbose == 3)
    cerr << "FITSArchive::unload ProcHistory exiting" << endl;
}
catch (Error& error)
{
  error += "Pulsar::FITSArchive::unload ProcHistory";
}

