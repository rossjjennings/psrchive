#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const WidebandCorrelator* ext)
{
  // status returned by FITSIO routines
  int status = 0;
  // no comment
  char* comment = 0;

  fits_update_key (fptr, TSTRING, "BECONFIG", 
		   const_cast<char*>(ext->configfile.c_str()),
		   comment, &status);
  
  fits_update_key (fptr, TINT, "NRCVR",
		   const_cast<int*>(&(ext->nrcvr)), comment, &status);

  fits_update_key (fptr, TDOUBLE, "TCYCLE",
		   const_cast<double*>(&(ext->tcycle)), comment, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload WidebandCorrelator");

}
