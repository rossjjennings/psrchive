#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "FITSError.h"

void unload (fitsfile* fptr, const Pulsar::ProcHistory::row* hrow)
{
  if (!hrow)
    return;

  int row = hrow->index;

  if (Pulsar::Archive::verbose)
    cerr << "unload Pulsar::ProcHistory::row row=" << row << endl;

  int status = 0;
  int colnum = 0;
  
  char*  tempstr = 0;
  int    tempint = 0;
  double tempdouble = 0;

  // AWH 17/2/2003 :
  
  // fits_read/write_col TSTRING data arguments must be of type char**
  // while fits_update_key TSTRING data args must be of type char*
  // very confusing, you have to be careful.
  
  // Write DATE_PRO
  
  fits_get_colnum (fptr, CASEINSEN, "DATE_PRO", &colnum, &status);
  
  // Write the value from the specified row
  
  tempstr = const_cast<char*>( hrow->date_pro.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0) {
    cerr << "WARNING: FITSArchive::unload_hist_row DATE_PRO not found" << endl;
    status = 0;
    //throw FITSError (status, "FITSArchive::unload_hist_row", 
    //       "fits_write_col DATE_PRO");
  }
  
  // Write PROC_CMD
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PROC_CMD", &colnum, &status);
  
  tempstr = const_cast<char*>( hrow->proc_cmd.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0) {
    cerr << "WARNING: FITSArchive::unload_hist_row PROC_CMD not found" << endl;
    status = 0;
    //throw FITSError (status, "FITSArchive::unload_hist_row", 
    //       "fits_write_col PROC_CMD");
  }
  
  // Write POL_TYPE
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "POL_TYPE", &colnum, &status);
  
  tempstr = const_cast<char*>( hrow->pol_type.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col POL_TYPE");
  
  // Write NPOL
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "NPOL", &colnum, &status);
  
  tempint = hrow->npol;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col NPOL");
  
  // Write NBIN
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "NBIN", &colnum, &status);
  
  tempint = hrow->nbin;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col NBIN");
  
  // Write NBIN_PRD
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "NBIN_PRD", &colnum, &status);
  
  tempint = hrow->nbin_prd;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col NBIN_PRD");
  
  // Write TBIN
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TBIN", &colnum, &status);
  
  tempdouble = hrow->tbin;
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col TBIN");
  
  // Write CTR_FREQ
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "CTR_FREQ", &colnum, &status);
  
  tempdouble = hrow->ctr_freq;
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col CTR_FREQ");

  // Write NCHAN
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "NCHAN", &colnum, &status);
  
  tempint = hrow->nchan;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col NCHAN");
  
  // Write CHAN_BW
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "CHAN_BW", &colnum, &status);
  
  tempdouble = hrow->chanbw;
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col CHAN_BW");
  
  // Write PAR_CORR
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PAR_CORR", &colnum, &status);
  
  tempint = hrow->par_corr;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col PAR_CORR");

  // Write RM_CORR
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "RM_CORR", &colnum, &status);
  
  tempint = hrow->rm_corr;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col RM_CORR");

  // Write DEDISP
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DEDISP", &colnum, &status);
  
  tempint = hrow->dedisp;
  fits_write_col (fptr, TINT, colnum, row, 1, 1, &tempint, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col DEDISP");
  
  // Write SC_MTHD

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "SC_MTHD", &colnum, &status);
  
  // Write the value from the specified row
  
  tempstr = const_cast<char*>( hrow->sc_mthd.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col SC_MTHD");
  
  // Write CAL_MTHD
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "CAL_MTHD", &colnum, &status);
  
  // Write the value from the specified row
  
  tempstr = const_cast<char*>( hrow->cal_mthd.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col CAL_MTHD");

  // Write CAL_FILE
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "CAL_FILE", &colnum, &status);
  
  // Write the value from the specified row
  
  tempstr = const_cast<char*>( hrow->cal_file.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_hist_row", 
                     "fits_write_col CAL_FILE");
  
  // Write RFI_MTHD
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "RFI_MTHD", &colnum, &status);
  
  // Write the value from the specified row
  
  tempstr = const_cast<char*>( hrow->rfi_mthd.c_str() );
  fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &tempstr, &status);
  
  if (status != 0) {
    cerr << "WARNING: FITSArchive::unload_hist_row RFI_MTHD not found" << endl;
    status = 0;
    //throw FITSError (status, "FITSArchive::unload_hist_row", 
    //       "fits_write_col RFI_MTHD");
  }

  if (Pulsar::Archive::verbose)
    cerr << "FITSArchive::unload_hist_row exiting" << endl;
}


void Pulsar::FITSArchive::unload (fitsfile* fptr, const ProcHistory* history)
{
  unsigned numrows = history->rows.size();

  if (verbose)
    cerr << "FITSArchive::unload ProcHistory rows=" << numrows << endl;

  // Move to the HISTORY HDU

  int status = 0;

  fits_movnam_hdu (fptr, BINARY_TBL, "HISTORY", 0, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_movnam_hdu HISTORY");
  
  fits_delete_rows (fptr, 1, 1, &status);
  
  fits_insert_rows (fptr, 0, numrows, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload ProcHistory",
                     "fits_insert_rows HISTORY");
  
  for (unsigned i = 0; i < numrows; i++) 
    ::unload (fptr, &(history->rows[i]));
  
  if (verbose)
    cerr << "FITSArchive::unload ProcHistory exiting" << endl;
}
