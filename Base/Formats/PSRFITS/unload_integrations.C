#include "Pulsar/FITSArchive.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload_integrations (fitsfile* ffptr) const
{
  int status = 0;

  // Move to the SUBINT Binary Table
  
  fits_movnam_hdu (ffptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "fits_movnam_hdu SUBINT");
      
  // Delete all information in the data HDU to ensure
  // no conflicts with the new state

  cerr << "FITSArchive::unload_integrations nsubint=" << nsubint << endl;

  long oldrownum = 0;

  fits_get_num_rows (ffptr, &oldrownum, &status);
  fits_delete_rows (ffptr, 1, oldrownum, &status);
  fits_insert_rows (ffptr, 0, nsubint, &status);

  if (verbose) {
    long newrownum = 0;
    fits_get_num_rows (ffptr, &newrownum, &status);
    if (verbose) {
      cerr << "FITSArchive::unload_integrations DATA row count = "
	   << newrownum
	   << endl;
    }
  }
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error clearing old subints");

  // Set the sizes of the columns which may have changed
  
  int colnum = 0;
  
  fits_get_colnum (ffptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_FREQ");

  if (verbose)
    cerr << "FITSArchive::unload_integrations DAT_FREQ resized to "
	 << nchan
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_WTS");

  if (verbose)
    cerr << "FITSArchive::unload_integrations DAT_WTS resized to "
	 << nchan
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_OFFS");

  if (verbose)
    cerr << "FITSArchive::unload_integrations DAT_OFFS resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_SCL");

  if (verbose)
    cerr << "FITSArchive::unload_integrations DAT_SCL resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol*nbin, &status);

  if (verbose)
    cerr << "FITSArchive::unload_integrations DATA resized to "
	 << nchan*npol*nbin
	 << endl;
  
  // Iterate over all rows, calling the unload_integration function to
  // fill in the next spot in the file.
  
  for(unsigned i = 0; i < nsubint; i++)
    unload_integration(i+1, get_Integration(i), ffptr);

  if (verbose)
    cerr << "FITSArchive::unload_integrations exit" << endl;

}
