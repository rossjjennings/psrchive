#include "Pulsar/CalibratorExtensionIO.h"
#include <limits>

#ifdef sun
#include <ieeefp.h>
#endif

void Pulsar::load (fitsfile* fptr, CalibratorExtension* ext)
{
  int status = 0;
 
  if (Archive::verbose == 3)
    cerr << "Pulsar::load CalibratorExtension" << endl;
  
  char* comment = 0;

  // Get NCHAN
  int nchan = 0;
  fits_read_key (fptr, TINT, "NCHAN", &nchan, comment, &status);

  if (status == 0)  {
    if (Archive::verbose == 3)
      cerr << "Pulsar::load CalibratorExtension NCHAN=" << nchan << endl;
    if (nchan >= 0)
      ext->set_nchan(nchan);
  }

  status = 0;

  // Not a fatal error if extension is empty
  if (ext->get_nchan() == 0) {
    if (Archive::verbose == 3) 
      cerr << "Pulsar::load CalibratorExtension HDU"
	   << " contains no data." << endl;
    return;
  }

  // Get EPOCH
  char* epoch = new char[80];
  fits_read_key (fptr, TSTRING, "EPOCH", epoch, comment, &status);

  // Also not a fatal error if not present
  if (status == 0) {
    MJD mjd (epoch);
    ext->set_epoch (mjd);
  }

  delete [] epoch;
  status = 0;

  long dimension = ext->get_nchan();

  auto_ptr<float> data ( new float[dimension] );
  
  // Read the centre frequencies
  
  int colnum = 0;
  int initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);

  float nullfloat = std::numeric_limits<float>::infinity();

  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nchan, &nullfloat,
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "Pulsar::load CalibratorExtension", 
		     "fits_read_col DAT_FREQ");

  if (Archive::verbose == 3) 
    cerr << "Pulsar::load CalibratorExtension frequencies read" << endl;

  unsigned ichan = 0;

  for (ichan=0; ichan < ext->get_nchan(); ichan++)
    ext->set_centre_frequency (ichan, data.get()[ichan]);

  // Read the data scale weights
  
  colnum = 0;
  initflag = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
 
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, nchan, &nullfloat, 
		 data.get(), &initflag, &status);

  if (status)
    throw FITSError (status, "Pulsar::load CalibratorExtension", 
		     "fits_read_col DAT_WTS");

  if (Archive::verbose == 3)
    cerr << "Pulsar::load CalibratorExtension weights read" << endl;

  for (int ichan=0; ichan < nchan; ichan++)
    if ( !finite(data.get()[ichan]) )
      data.get()[ichan] = 0;

  for (ichan=0; ichan < ext->get_nchan(); ichan++)
    ext->set_weight (ichan, data.get()[ichan]);

  if (Archive::verbose == 3)
    cerr << "Pulsar::load CalibratorExtension exit." << endl;
}
