#include "Pulsar/FITSArchive.h"
#include "Pulsar/ITRFExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::load_ITRFExtension (fitsfile* fptr)
{
  // status returned by FITSIO routines
  int status = 0;
  // no comment
  char* comment = 0;

  if (verbose)
    cerr << "FITSArchive::load_ITRFExtension" << endl;
  
  Reference::To<ITRFExtension> ext = new ITRFExtension;

  fits_read_key (fptr, TDOUBLE, "ANT_X", &(ext->ant_x), comment, &status);
  fits_read_key (fptr, TDOUBLE, "ANT_Y", &(ext->ant_y), comment, &status);
  fits_read_key (fptr, TDOUBLE, "ANT_Z", &(ext->ant_z), comment, &status);

  if (status == 0) {
    add_extension (ext);
    return;
  }

  if (verbose)
    cerr << FITSError (status, "FITSArchive::load_ITRFExtension",
		       "fits_read_key ANT_X,Y,Z").warning() << endl;

}
