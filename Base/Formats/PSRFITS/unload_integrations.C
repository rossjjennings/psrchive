#include "Pulsar/FITSArchive.h"
#include "Pulsar/IntegrationOrder.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload_integrations (fitsfile* ffptr) const
{
  int status = 0;
  char* comment = 0;

  // Move to the SUBINT Binary Table
  
  fits_movnam_hdu (ffptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "fits_movnam_hdu SUBINT");
      
  // Delete all information in the data HDU to ensure
  // no conflicts with the new state

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations nsubint=" << nsubint << endl;

  long oldrownum = 0;

  fits_get_num_rows (ffptr, &oldrownum, &status);
  fits_delete_rows (ffptr, 1, oldrownum, &status);
  fits_insert_rows (ffptr, 0, nsubint, &status);

  if (verbose == 3) {
    long newrownum = 0;
    fits_get_num_rows (ffptr, &newrownum, &status);
    if (verbose == 3) {
      cerr << "FITSArchive::unload_integrations DATA row count = "
	   << newrownum
	   << endl;
    }
  }
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error clearing old subints");

  // Update the header information
  
  bool   has_alt_order = false;
  string order_name    = "unknown";
  string order_unit    = "unknown";

  if (get<Pulsar::IntegrationOrder>()) {
    has_alt_order = true;
    order_name = get<Pulsar::IntegrationOrder>()->get_extension_name();
    order_unit = get<Pulsar::IntegrationOrder>()->get_Unit();
  }

  if (has_alt_order) {
    fits_update_key (ffptr, TSTRING, "INT_TYPE", 
		     const_cast<char*>(order_name.c_str()),
		     comment, &status);
    
    fits_update_key (ffptr, TSTRING, "INT_UNIT", 
		     const_cast<char*>(order_unit.c_str()),
		     comment, &status);
  }
  else {
    char* useful = new char[64];
    sprintf(useful, "%s", "TIME");
    fits_update_key (ffptr, TSTRING, "INT_TYPE", useful,
		     comment, &status);
    
    sprintf(useful, "%s", "SEC");
    fits_update_key (ffptr, TSTRING, "INT_UNIT", useful,
		     comment, &status);
    delete[] useful;
  }
  
  // Set the sizes of the columns which may have changed
  
  int colnum = 0;
  
  fits_get_colnum (ffptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_FREQ");

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations DAT_FREQ resized to "
	 << nchan
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_WTS");

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations DAT_WTS resized to "
	 << nchan
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_OFFS");

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations DAT_OFFS resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_integrations", 
		     "error resizing DAT_SCL");

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations DAT_SCL resized to "
	 << nchan*npol
	 << endl;

  fits_get_colnum (ffptr, CASEINSEN, "DATA", &colnum, &status);
  fits_modify_vector_len (ffptr, colnum, nchan*npol*nbin, &status);

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations DATA resized to "
	 << nchan*npol*nbin
	 << endl;
  
  // Iterate over all rows, calling the unload_integration function to
  // fill in the next spot in the file.
  
  for(unsigned i = 0; i < nsubint; i++)
    unload_integration(i+1, get_Integration(i), ffptr);

  if (verbose == 3)
    cerr << "FITSArchive::unload_integrations exit" << endl;

}
