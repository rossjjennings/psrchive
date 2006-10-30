/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "Pulsar/Receiver.h"

#include "FITSError.h"

using namespace std;

void load (fitsfile* fptr, Pulsar::ProcHistory::row* hrow)
{
  int row = hrow->index;
  
  int status = 0;

  if (Pulsar::Archive::verbose == 3)
    cerr << "load ProcHistory::row entered" << endl;

  int colnum = 0;
  int initflag = 0;
  int nullint = 0;
  double nulld = 0.0;
  static char* nullstr = strdup(" ");
  
  auto_ptr<char> temp( new char[128] );
  char* temp_ptr = temp.get();

  // Get DATE_PRO
  
  fits_get_colnum (fptr, CASEINSEN, "DATE_PRO", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, nullstr, 
                 &temp_ptr, &initflag, &status);
  
  if (status != 0) {
    if (Pulsar::Archive::verbose)
      cerr << "load ProcHistory::row WARNING DATE_PRO not found" << endl;
    hrow->date_pro = "";
    status = 0;
  }
  else {
    if (Pulsar::Archive::verbose == 3)
      cerr << "Read DATE_PRO = " << temp.get() << endl;
    hrow->date_pro = temp.get();
  }
  
  // Get PROC_CMD
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "PROC_CMD", &colnum, &status);
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read PROC_CMD = " << temp.get() << endl;
  hrow->proc_cmd = temp.get();

  if (status != 0) {
    if (Pulsar::Archive::verbose)
      cerr << "load ProcHistory::row WARNING PROC_CMD not found" << endl;
    status = 0;
    hrow->proc_cmd = "";
  }
  else {
    if (Pulsar::Archive::verbose == 3)
      cerr << "Read PROC_CMD = " << temp.get() << endl;
    hrow->proc_cmd = temp.get();
  }

  // Get POL_TYPE
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "POL_TYPE", &colnum, &status);
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read POL_TYPE = " << temp.get() << endl;
  hrow->pol_type = temp.get();
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col POL_TYPE");
  
  // Get NPOL
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "NPOL", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->npol), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col NPOL");
  
  // Get NBIN
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "NBIN", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->nbin), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col NBIN");
  
  // Get NBIN_PRD
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "NBIN_PRD", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->nbin_prd), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col NBIN_PRD");
  
  // Get TBIN
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "TBIN", &colnum, &status);
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulld, 
                 &(hrow->tbin), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col TBIN");
  
  // Get CTR_FREQ
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "CTR_FREQ", &colnum, &status);
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulld, 
                 &(hrow->ctr_freq), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CTR_FREQ");
  
  // Get NCHAN
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "NCHAN", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->nchan), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col NCHAN");
  // Get CHAN_BW
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "CHAN_BW", &colnum, &status);
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulld, 
                 &(hrow->chanbw), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CHAN_BW");
  
  // Get PAR_CORR
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "PAR_CORR", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->par_corr), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col PAR_CORR");
  
  // Get FA_CORR
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "FA_CORR", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->fa_corr), &initflag, &status);
  
  if (status != 0) {
    if (Pulsar::Archive::verbose > 1)
      cerr << "load ProcHistory::row WARNING assuming uncorrected feed angle"
	   << endl;
    hrow->fa_corr = 0;
  }
  
  // Get RM_CORR
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "RM_CORR", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->rm_corr), &initflag, &status);
  
  if (status != 0)  {
    if (Pulsar::Archive::verbose > 1)
      cerr << "load ProcHistory::row WARNING assuming uncorrected RM" << endl;
    hrow->rm_corr = 0;
    status = 0;
  }
  
  // Get DEDISP
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DEDISP", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->dedisp), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col DEDISP");
  
  // Get SC_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "SC_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read SC_MTHD = " << temp.get() << endl;
  hrow->sc_mthd = temp.get();
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col SC_MTHD");
  
  // Get CAL_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "CAL_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read CAL_MTHD = " << temp.get() << endl;
  hrow->cal_mthd = temp.get();
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CAL_MTHD");
  
  // Get CAL_FILE
  
  fits_get_colnum (fptr, CASEINSEN, "CAL_FILE", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read CAL_FILE = " << temp.get() << endl;
  hrow->cal_file = temp.get();

  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CAL_FILE");
  
  // Get RFI_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "RFI_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read RFI_MTHD = " << temp.get() << endl;
  hrow->rfi_mthd = temp.get();

  if (status != 0) {
    if (Pulsar::Archive::verbose)
      cerr << "load ProcHistory::row WARNING RFI_MTHD not found" << endl;
    status = 0;
    //throw FITSError (status, "load ProcHistory::row", 
    //       "fits_read_col RFI_MTHD");
  }

  // Get IFR_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "IFR_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);
  if (Pulsar::Archive::verbose == 3)
    cerr << "Read IFR_MTHD = " << temp.get() << endl;
  hrow->ifr_mthd = temp.get();

  if (status != 0) {
    if (Pulsar::Archive::verbose == 3)
      cerr << "load ProcHistory::row WARNING IFR_MTHD not found" << endl;
    status = 0;
    //throw FITSError (status, "load ProcHistory::row", 
    //       "fits_read_col IFR_MTHD");
  }

  // Get SCALE
  
  fits_get_colnum (fptr, CASEINSEN, "SCALE", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp_ptr, &initflag, &status);

  if (status != 0) {
    
    if (Pulsar::Archive::verbose > 1)
      cerr << "load ProcHistory::row error reading SCALE" <<endl;
    status = 0;
    hrow->scale = Signal::FluxDensity;
    
  }
  else {

    if (Pulsar::Archive::verbose == 3)
      cerr << "load ProcHistory::row SCALE = " << temp.get() << endl;

    if (strcmp (temp.get(), "FluxDen") == 0)
      hrow->scale = Signal::FluxDensity;
    else if (strcmp (temp.get(), "RefFlux") == 0)
      hrow->scale = Signal::ReferenceFluxDensity;
    else if (strcmp (temp.get(), "Jansky") == 0)
      hrow->scale = Signal::Jansky;
    else if (Pulsar::Archive::verbose)
      cerr << "load ProcHistory::row WARNING unrecognized SCALE" << endl;
  }

  if (Pulsar::Archive::verbose == 3)
    cerr << "load ProcHistory::row exiting" << endl;
}

////////////////////////////
// ProcHistory I/O routines
//

void Pulsar::FITSArchive::load_ProcHistory (fitsfile* fptr)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::load_ProcHistory entered" << endl;

  // some processing flags are stored in the Receiver extension
  Reference::To<Receiver> receiver = get<Receiver>();
  if (!receiver)
    throw Error (InvalidState, "FITSArchive::load_ProcHistory", 
		 "no Receiver Extension: first call load_Receiver");

  // Move to the HISTORY HDU

  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_ProcHistory", 
                     "fits_movnam_hdu HISTORY");


  Reference::To<ProcHistory> history = new ProcHistory;

  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_ProcHistory", 
                     "fits_get_num_rows HISTORY");

  (history->rows).resize(numrows);   
  
  for (int i = 0; i < numrows; i++) {
    history->rows[i] = ProcHistory::row();
    history->rows[i].index = i+1;

    ::load( fptr, &(history->rows[i]) );
  }

  set_nbin (history->get_last().nbin);
  set_npol (history->get_last().npol);
  set_centre_frequency (history->get_last().ctr_freq);
  set_nchan (history->get_last().nchan);

  chanbw = history->get_last().chanbw;
  set_bandwidth(get_nchan()*chanbw);

  if (history->get_last().cal_mthd == "NONE")
    set_poln_calibrated (false);
  else {
    set_poln_calibrated (true);
    history->set_cal_mthd(history->get_last().cal_mthd);
  }

  history->set_sc_mthd (history->get_last().sc_mthd);
  history->set_cal_file(history->get_last().cal_file);
  history->set_rfi_mthd(history->get_last().rfi_mthd);
  history->set_ifr_mthd(history->get_last().ifr_mthd);

  set_scale ( history->get_last().scale );
  string polstr = history->get_last().pol_type;

  if(polstr == "XXYY") {
    set_state ( Signal::PPQQ );
    if (verbose == 3)
      cerr << "FITSArchive:load_header setting Signal::PPQQ" << endl;
  }
  else if(polstr == "STOKE") {
    set_state ( Signal::Stokes );
    if (verbose == 3)
      cerr << "FITSArchive:load_header setting Signal::Stokes" << endl;
  }
  else if(polstr == "XXYYCRCI") {
    set_state ( Signal::Coherence );
    if (verbose == 3)
      cerr << "FITSArchive:load_header setting Signal::Coherence" << endl;
  }
  else if(polstr == "INTEN") {
    set_state ( Signal::Intensity );
    if (verbose == 3)
      cerr << "FITSArchive:load_header setting Signal::Intensity" << endl;
  }
  else if(polstr == "INVAR")
    set_state ( Signal::Invariant );
  else {
    if (verbose == 3) {
      cerr << "FITSArchive:load_header WARNING unknown POL_TYPE = "
           << polstr <<endl;
      cerr << "FITSArchive:load_header setting Signal::Intensity"
           << endl;
    }
    set_state ( Signal::Intensity );
  }

  //
  // WvS 23-06-2004
  //
  // Note that ionospheric rotation measure has been removed from the
  // Pulsar::Archive definition.  When implemented, it will be treated
  // as an Extension.
  
  if (history->get_last().rm_corr == 1)
    set_faraday_corrected (true);

  else if (history->get_last().rm_corr == 0)
    set_faraday_corrected (false);

  else {
    if (verbose == 3) {
      cerr << "FITSArchive:load_header unexpected value in RM_CORR flag"
           << endl;
    }
    set_faraday_corrected (false);
  }

  if (history->get_last().par_corr == 1)
    receiver->set_platform_corrected (true);

  else if (history->get_last().par_corr == 0)
    receiver->set_platform_corrected (false);

  else {
    if (verbose == 3)
      cerr << "FITSArchive::load_header unexpected PAR_CORR flag"
           << endl;

    receiver->set_platform_corrected (false);
  }


  if (history->get_last().fa_corr == 1)
    receiver->set_feed_corrected (true);

  else if (history->get_last().fa_corr == 0)
    receiver->set_feed_corrected (false);

  else {
    if (verbose == 3) {
      cerr << "FITSArchive::load_header unexpected FA_CORR flag"
           << endl;
    }
    receiver->set_feed_corrected (false);
  }


  if(history->get_last().dedisp == 1)
    set_dedispersed (true);
  else if(history->get_last().dedisp == 0)
    set_dedispersed (false);
  else {
    if (verbose == 3) {
      cerr << "FITSArchive::load unexpected DEDISP flag"
           << endl;
    }
    set_dedispersed (false);
  }

  add_extension (history);

  if (verbose == 3)
    cerr << "FITSArchive::load_ProcHistory exiting" << endl;
}


