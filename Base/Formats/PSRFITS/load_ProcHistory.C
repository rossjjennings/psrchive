#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "FITSError.h"

void load (fitsfile* fptr, Pulsar::ProcHistory::row* hrow)
{
  int row = hrow->index;
  
  int status = 0;

  if (Pulsar::Archive::verbose)
    cerr << "load ProcHistory::row entered" << endl;

  int colnum = 0;
  int initflag = 0;
  int nullint = 0;
  double nulld = 0.0;
  static char* nullstr = strdup(" ");
  
  char* temp = new char[128];

  // Get DATE_PRO
  
  fits_get_colnum (fptr, CASEINSEN, "DATE_PRO", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read DATE_PRO = " << temp << endl;
  hrow->date_pro = temp;
  
  if (status != 0) {
    cerr << "WARNING: load ProcHistory::row DATE_PRO not found" 
         << endl;
    status = 0;
    //throw FITSError (status, "load ProcHistory::row", 
    //       "fits_read_col DATE_PRO");
  }
  
  // Get PROC_CMD
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "PROC_CMD", &colnum, &status);
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read PROC_CMD = " << temp << endl;
  hrow->proc_cmd = temp;

  if (status != 0) {
    cerr << "WARNING: load ProcHistory::row PROC_CMD not found" 
         << endl;
    status = 0;
    //throw FITSError (status, "load ProcHistory::row", 
    //       "fits_read_col PROC_CMD");
  }
  
  // Get POL_TYPE
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "POL_TYPE", &colnum, &status);
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read POL_TYPE = " << temp << endl;
  hrow->pol_type = temp;
  
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
  
  // Get RM_CORR
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "RM_CORR", &colnum, &status);
  
  fits_read_col (fptr, TINT, colnum, row, 1, 1, &nullint, 
                 &(hrow->rm_corr), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col RM_CORR");
  
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
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read SC_MTHD = " << temp << endl;
  hrow->sc_mthd = temp;
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col SC_MTHD");
  
  // Get CAL_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "CAL_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read CAL_MTHD = " << temp << endl;
  hrow->cal_mthd = temp;
  
  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CAL_MTHD");
  
  // Get CAL_FILE
  
  fits_get_colnum (fptr, CASEINSEN, "CAL_FILE", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read CAL_FILE = " << temp << endl;
  hrow->cal_file = temp;

  if (status != 0)
    throw FITSError (status, "load ProcHistory::row", 
                     "fits_read_col CAL_FILE");
  
  // Get RFI_MTHD
  
  fits_get_colnum (fptr, CASEINSEN, "RFI_MTHD", &colnum, &status);
  
  // Read the value from the specified row
  
  fits_read_col (fptr, TSTRING, colnum, row, 1, 1, &nullstr, 
                 &temp, &initflag, &status);
  if (Pulsar::Archive::verbose)
    cerr << "Read RFI_MTHD = " << temp << endl;
  hrow->rfi_mthd = temp;

  if (status != 0) {
    cerr << "WARNING: load ProcHistory::row RFI_MTHD not found" 
         << endl;
    status = 0;
    //throw FITSError (status, "load ProcHistory::row", 
    //       "fits_read_col RFI_MTHD");
  }

  delete[] temp;

  if (Pulsar::Archive::verbose)
    cerr << "load ProcHistory::row exiting" << endl;
}

////////////////////////////
// ProcHistory I/O routines
//

void Pulsar::FITSArchive::load_ProcHistory (fitsfile* fptr)
{
  int status = 0;

  if (verbose)
    cerr << "FITSArchive::load_ProcHistory entered" << endl;
  
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

  set_nbin ((history->get_last()).nbin);
  set_npol ((history->get_last()).npol);
  set_centre_frequency ((history->get_last()).ctr_freq);
  set_nchan ((history->get_last()).nchan);

  chanbw = (history->get_last()).chanbw;
  set_bandwidth(get_nchan()*chanbw);

  if ((history->get_last()).cal_mthd == "SingleAxis" || (history->get_last()).cal_mthd == "SelfCAL" ||
      (history->get_last()).cal_mthd == "Polar" || (history->get_last()).cal_mthd == "Other") {
    set_poln_calibrated(true);
    history->set_cal_mthd((history->get_last()).cal_mthd);
  }
  else {
    set_poln_calibrated(false);
  }

  if ((history->get_last()).sc_mthd == "PAC") {
    set_flux_calibrated(true);
    history->set_sc_mthd("PAC");
  }
  else {
    set_flux_calibrated(false);
  }

  history->set_cal_file((history->get_last()).cal_file);
  history->set_rfi_mthd((history->get_last()).rfi_mthd);

  string polstr = (history->get_last()).pol_type;

  if(polstr == "XXYY") {
    set_state ( Signal::PPQQ );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::PPQQ" << endl;
  }
  else if(polstr == "STOKE") {
    set_state ( Signal::Stokes );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Stokes" << endl;
  }
  else if(polstr == "XXYYCRCI") {
    set_state ( Signal::Coherence );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Coherence" << endl;
  }
  else if(polstr == "INTEN") {
    set_state ( Signal::Intensity );
    if (verbose)
      cerr << "FITSArchive:load_header setting Signal::Intensity" << endl;
  }
  else if(polstr == "INVAR")
    set_state ( Signal::Invariant );
  else {
    if (verbose) {
      cerr << "FITSArchive:load_header WARNING unknown POL_TYPE = "
           << polstr <<endl;
      cerr << "FITSArchive:load_header setting Signal::Intensity"
           << endl;
    }
    set_state ( Signal::Intensity );
  }


  if((history->get_last()).rm_corr == 1) {
    set_ism_rm_corrected (true);
    set_iono_rm_corrected (true);
  }
  else if((history->get_last()).rm_corr == 0) {
    set_ism_rm_corrected (false);
    set_iono_rm_corrected (false);
  }
  else {
    if (verbose) {
      cerr << "FITSArchive:load_header unexpected value in RM_CORR flag"
           << endl;
    }
    set_iono_rm_corrected (false);
    set_ism_rm_corrected (false);
  }

  if (verbose)
    cerr << "FITSArchive::load_header WARNING rotation measure ambiguity"
         << endl;

  //
  // The Pulsar::Archive class has two rotation measure correction
  // flags, one for the ionosphere and one for the interstellar
  // medium. The FITS definition lumps them both together. This isn't
  // really a problem in the loader, because if the FITS correction
  // flag is set, both the Pulsar::Archive flags will need to be set.
  //
  // Worth noting though.
  //

  if((history->get_last()).par_corr == 1) {
    set_parallactic_corrected (true);
    set_feedangle_corrected (false);
  }
  else if((history->get_last()).par_corr == 0) {
    set_feedangle_corrected (false);
    set_parallactic_corrected (false);
  }
  else {
    if (verbose) {
      cerr << "FITSArchive::load_header unexpected PAR_CORR flag"
           << endl;
    }
    set_parallactic_corrected (false);
    set_feedangle_corrected(false);
  }

  if (verbose)
    cerr << "FITSArchive::load WARNING, assuming PA_CORR implies FA_CORR"
         << endl;

  if((history->get_last()).dedisp == 1)
    set_dedispersed (true);
  else if((history->get_last()).dedisp == 0)
    set_dedispersed (false);
  else {
    if (verbose) {
      cerr << "FITSArchive::load unexpected DEDISP flag"
           << endl;
    }
    set_dedispersed (false);
  }

  add_extension (history);

  if (verbose)
    cerr << "FITSArchive::load_ProcHistory exiting" << endl;
}


