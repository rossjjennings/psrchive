#include "FITSError.h"

#include <stdarg.h>
#include <fitsio.h>

FITSError::FITSError (int status, const char* func, const char* msg=0, ...)
{
  char buf[1024];
  va_list args;
  
  va_start(args, msg);
  vsnprintf(buf, 1024, msg, args);
  va_end(args);
  message = buf;

  char fits_error[FLEN_ERRMSG];

  fits_get_errstatus (status, fits_error);
  message += ": ";
  message += fits_error;

  code = FailedCall; 

  functions.push_back(func);
} 
