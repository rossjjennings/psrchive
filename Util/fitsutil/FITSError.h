//-*-C++-*-

#ifndef __FITSError_h
#define __FITSError_h

#include "Error.h"

class FITSError : public Error {

  public:

  //! Error with optional printf-style message
  FITSError (int status, const char* func, const char* msg=0, ...);

  //! Error with string message
  FITSError (int status, const char* func, const string& msg);

  //! Destructor
  ~FITSError () {}

};

#endif
