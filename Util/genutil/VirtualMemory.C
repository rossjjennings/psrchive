/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "VirtualMemory.h"
#include "Error.h"

#include <sys/mman.h>

using namespace std;

//! Construct with regular expression
VirtualMemory::VirtualMemory (const string& filename)
  : TemporaryFile (filename)
{
  page_size = (size_t) sysconf (_SC_PAGESIZE);
  length = 0;
}

//! Destructor
VirtualMemory::~VirtualMemory ()
{
  destroy ();
}

void VirtualMemory::destroy ()
{

}

void* VirtualMemory::mmap (size_t length)
{

}

