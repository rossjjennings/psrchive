//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/VirtualMemory.h,v $
   $Revision: 1.1 $
   $Date: 2006/12/14 22:50:50 $
   $Author: straten $ */

#ifndef __VirtualMemory_h
#define __VirtualMemory_h

#include "TemporaryFile.h"

//! Virtual memory manager
class VirtualMemory : public TemporaryFile {

 public:

  //! Construct a virutal memory resource with the given filename
  VirtualMemory (const std::string& filename);

  //! Destructor
  ~VirtualMemory ();

  //! Map the specified number of bytes into memory
  void* mmap (size_t length);

 private:

  //! The page size used for swapping
  size_t page_size;

  //! The total number of bytes mapped
  size_t length;

  //! Cleanup all resources
  void destroy ();

};

#endif

