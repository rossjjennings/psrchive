//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/FilePtr.h

#ifndef __FilePtr_H
#define __FilePtr_H

#include <stdio.h>
#include <string>

//! Closes a FILE* when it goes out of scope
class FilePtr
{
public:

  FilePtr (const std::string& filename, const std::string& mode);
  FilePtr (FILE*);
  FilePtr () { fptr = 0; }
  ~FilePtr ();
  const FilePtr& operator = (FILE*);
  operator FILE* () { return fptr; }
  bool operator ! () { return fptr == 0; }

protected:
  void close ();
  FILE* fptr;
};

#endif

