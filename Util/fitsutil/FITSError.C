#include "FITSError.h"

#include <stdarg.h>
#include <fitsio.h>

FITSError::FITSError (int status, const char* func, const char* msg, ...)
{
  char buf[1024];
  string this_msg;

  if (msg) {
    va_list args;
  
    va_start(args, msg);
    vsnprintf(buf, 1024, msg, args);
    va_end(args);
    this_msg = buf;
  }

  char fits_error[FLEN_ERRMSG];
  fits_get_errstatus (status, fits_error);
  this_msg += ": ";
  this_msg += fits_error;

  construct (FailedCall, func, this_msg.c_str());
} 

