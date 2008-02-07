/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/Receiver.h"

#include "psrfitsio.h"

using namespace std;

void load (fitsfile* fptr, Pulsar::ProcHistory::row* hrow )
{
  int row = hrow->index;
  
  if (Pulsar::Archive::verbose > 2)
    cerr << "load ProcHistory::row entered" << endl;

  string empty = "";

  //
  // in the following, if a value cannot be read from file:
  // A) if no default is given, an exception will be thrown; or
  // B) if a default is given, the value will be set to it
  //

  psrfits_read_col (fptr, "DATE_PRO", &(hrow->date_pro), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  if (Pulsar::Archive::verbose > 2)
    cerr << "load ProcHistory::row DATE_PRO=" << hrow->date_pro << endl;

  psrfits_read_col (fptr, "PROC_CMD", &(hrow->proc_cmd), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  if (Pulsar::Archive::verbose > 2)
    cerr << "load ProcHistory::row PROC_CMD=" << hrow->proc_cmd << endl;

  psrfits_read_col (fptr, "POL_TYPE", &(hrow->pol_type), row);

  psrfits_read_col (fptr, "NSUB", &(hrow->nsub), row,
		    0, 0, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "NPOL", &(hrow->npol), row);

  psrfits_read_col (fptr, "NBIN", &(hrow->nbin), row);

  psrfits_read_col (fptr, "NBIN_PRD", &(hrow->nbin_prd), row, 
		    0, hrow->nbin, Pulsar::Archive::verbose > 2);

  double zero = 0.0;
  psrfits_read_col (fptr, "TBIN", &(hrow->tbin), row, 
		    zero, zero, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "CTR_FREQ", &(hrow->ctr_freq), row);

  psrfits_read_col (fptr, "NCHAN", &(hrow->nchan), row);

  psrfits_read_col (fptr, "CHAN_BW", &(hrow->chan_bw), row);

  psrfits_read_col (fptr, "PAR_CORR", &(hrow->par_corr), row,
		    0, 0, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "FA_CORR", &(hrow->fa_corr), row,
		    0, 0, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "RM_CORR", &(hrow->rm_corr), row,
		    0, 0, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "DEDISP", &(hrow->dedisp), row,
		    0, 0, Pulsar::Archive::verbose > 2);
  
  empty = "NONE";
  
  psrfits_read_col (fptr, "DDS_MTHD", &(hrow->dds_mthd), row,
		    empty, empty, Pulsar::Archive::verbose > 2 );

  psrfits_read_col (fptr, "SC_MTHD", &(hrow->sc_mthd), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "CAL_MTHD", &(hrow->cal_mthd), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "CAL_FILE", &(hrow->cal_file), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "RFI_MTHD", &(hrow->rfi_mthd), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "IFR_MTHD", &(hrow->ifr_mthd), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  psrfits_read_col (fptr, "SCALE", &(hrow->scale), row,
		    empty, empty, Pulsar::Archive::verbose > 2);

  // scale string interpretation is now done in load_integrations

  if (Pulsar::Archive::verbose > 2)
    cerr << "load ProcHistory::row exiting" << endl;
}

////////////////////////////
// ProcHistory I/O routines
//

void Pulsar::FITSArchive::load_ProcHistory (fitsfile* fptr)
{
  if (verbose > 2)
    cerr << "FITSArchive::load_ProcHistory entered" << endl;

  // some processing flags are stored in the Receiver extension
  Reference::To<Receiver> receiver = get<Receiver>();
  if (!receiver)
    throw Error (InvalidState, "FITSArchive::load_ProcHistory", 
		 "no Receiver Extension: first call load_Receiver");

  // Move to the HISTORY HDU

  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);
  
  if (status != 0) {
    if (verbose > 2)
      cerr << "FITSArchive::load_ProcHistory no HISTORY table" << endl;
    return;
  }

  Reference::To<ProcHistory> history = new ProcHistory;

  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_ProcHistory", 
                     "fits_get_num_rows HISTORY");
  
  history->rows.resize(numrows);   
  
  if (!numrows)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_ProcHistory no rows" << endl;
    return;
  }

  for (int i = 0; i < numrows; i++)
  {
    history->rows[i] = ProcHistory::row();
    history->rows[i].index = i+1;

    ::load( fptr, &(history->rows[i]) );
  }

  ProcHistory::row& last = history->get_last();

  /*
    WvS - 07 Feb 2008
    Beginning with PSRFITS version 3, the centre frequency and
    bandwidth are stored in the primary header as OBSFREQ and OBSBW.
    These values are over-ridden by the history, if available.
  */
  set_centre_frequency (last.ctr_freq);
  set_bandwidth (last.nchan * last.chan_bw);

  /*
    WvS - 07 Feb 2008
    Beginning with PSRFITS version 3, the number of polarizations,
    frequency channels, and phase bins are stored in the SUBINT HDU.
    However, prior to version 3, these attributes may not be reliable.
    They are set here with the understanding that they may be reset
    during load_integrations.
  */
  set_npol  (last.npol);
  set_nchan (last.nchan);
  set_nbin  (last.nbin);

  if (last.cal_mthd == "NONE")
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_header not calibrated" << endl;
    set_poln_calibrated (false);
  }
  else {
    if (verbose > 2)
      cerr << "FITSArchive::load_header calibrated" << endl;
    set_poln_calibrated (true);
    history->set_cal_mthd(last.cal_mthd);
  }

  history->set_sc_mthd (last.sc_mthd);
  history->set_cal_file(last.cal_file);
  history->set_rfi_mthd(last.rfi_mthd);
  history->set_ifr_mthd(last.ifr_mthd);

  //
  // WvS 23 Jun 2004
  //
  // Note that ionospheric rotation measure has been removed from the
  // Pulsar::Archive definition.  When implemented, it will be treated
  // as an Extension.
  
  if (last.rm_corr == 1)
    set_faraday_corrected (true);

  else if (last.rm_corr == 0)
    set_faraday_corrected (false);

  else {
    if (verbose > 2) {
      cerr << "FITSArchive:load_header unexpected value in RM_CORR flag"
           << endl;
    }
    set_faraday_corrected (false);
  }

  if (last.par_corr == 1)
    receiver->set_platform_corrected (true);

  else if (last.par_corr == 0)
    receiver->set_platform_corrected (false);

  else {
    if (verbose > 2)
      cerr << "FITSArchive::load_header unexpected PAR_CORR flag"
           << endl;

    receiver->set_platform_corrected (false);
  }


  if (last.fa_corr == 1)
    receiver->set_feed_corrected (true);

  else if (last.fa_corr == 0)
    receiver->set_feed_corrected (false);

  else {
    if (verbose > 2) {
      cerr << "FITSArchive::load_header unexpected FA_CORR flag"
           << endl;
    }
    receiver->set_feed_corrected (false);
  }


  if(last.dedisp == 1)
    set_dedispersed (true);
  else if(last.dedisp == 0)
    set_dedispersed (false);
  else {
    if (verbose > 2) {
      cerr << "FITSArchive::load unexpected DEDISP flag"
           << endl;
    }
    set_dedispersed (false);
  }

  add_extension (history);

  if (verbose > 2)
    cerr << "FITSArchive::load_ProcHistory exiting" << endl;

}


