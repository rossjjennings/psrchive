#include "Pulsar/FITSArchive.h"
#include "Pulsar/ObsExtension.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const ObsExtension* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // do not return comments in fits_read_key
  char* comment = 0;

  fits_update_key (fptr, TSTRING, "OBSERVER", 
		   (char*)(ext->observer.c_str()), comment, &status);

  fits_update_key (fptr, TSTRING, "PROJID", 
		   (char*)(ext->project_ID.c_str()), comment, &status);
  
  fits_update_key (fptr, TSTRING, "TELESCOP", 
  		   (char*)(ext->telescope.c_str()), comment, &status);

}
