//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/FilePtr.h,v $
   $Revision: 1.2 $
   $Date: 2009/03/15 06:55:53 $
   $Author: straten $ */

#ifndef __FilePtr_H
#define __FilePtr_H

#include <stdio.h>
#include <string>

//! Closes a FILE* when it goes out of scope
/*! This may be the most useful class ever written */
class FilePtr
{
public:

  FilePtr (const std::string& filename, const std::string& mode);

  FilePtr (FILE* f) { fptr = f; }
  ~FilePtr () { if (fptr) fclose (fptr); }
  operator FILE* () { return fptr; }
  bool operator ! () { return fptr == 0; }

protected:
  FILE* fptr;
};

#endif

