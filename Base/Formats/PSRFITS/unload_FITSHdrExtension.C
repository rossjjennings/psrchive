/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include "FITSError.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const FITSHdrExtension* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // standard string length defined in fitsio.h
  char error[FLEN_ERRMSG];

  // do not return comments in fits_read_key
  char* comment = 0;

  if(status != 0) {
    fits_get_errstatus(status,error);
    if (verbose == 3)
      cerr << "WARNING: FITSArchive::unload_file MJD - " << error << endl;
    status = 0;
  }
  
  // Write pulsar position data
  
  fits_update_key (fptr, TSTRING, "COORD_MD",
		   (char*)(ext->coordmode.c_str()), comment, &status);

  fits_update_key (fptr, TSTRING, "DATE", 
		   (char*)(ext->creation_date.c_str()), comment, &status);
  

  fits_update_key (fptr, TSTRING, "TRK_MODE", 
  		   (char*)(ext->trk_mode.c_str()), comment, &status);
  

  fits_update_key (fptr, TSTRING, "STT_DATE", 
  		   (char*)(ext->stt_date.c_str()), comment, &status);
  
  fits_update_key (fptr, TSTRING, "STT_TIME", 
  		   (char*)(ext->stt_time.c_str()), comment, &status);
  
  fits_update_key (fptr, TDOUBLE, "STT_LST", (double*)&(ext->stt_lst), 
		   comment, &status);

}

