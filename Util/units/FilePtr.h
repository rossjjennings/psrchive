//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/FilePtr.h,v $
   $Revision: 1.1 $
   $Date: 2009/03/02 14:50:17 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_PAD_H
#define __UTILS_UNITS_PAD_H

#include <stdio.h>

//! Closes a FILE* when it goes out of scope
/*! This may be the most useful class ever written */
class FilePtr
{
public:
  FilePtr (FILE* f) { fptr = f; }
  ~FilePtr () { if (fptr) fclose (fptr); }
  operator FILE* () { return fptr; }
  bool operator ! () { return fptr == 0; }
protected:
  FILE* fptr;
};

#endif

