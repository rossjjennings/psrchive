/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VirtualMemory.h"
#include "Error.h"

#include <unistd.h>
#include <sys/mman.h>

using namespace std;

#include <algorithm>

//! Construct with regular expression
VirtualMemory::VirtualMemory (const string& filename)
  : TemporaryFile (filename)
{
  // cerr << "VirtualMemory ctor filename=" << get_filename() << endl;

  // so the swap file will automatically be deleted when the program exists
  unlink ();

  swap_space = 0;

#ifdef HAVE_PTHREAD
  context = new ThreadContext;
#else
  context = 0;
#endif
}

//! Destructor
VirtualMemory::~VirtualMemory ()
{
}

//! Map the specified number of bytes into memory
void* VirtualMemory::mmap (size_t size) try
{
  // cerr << "VirtualMemory::mmap size=" << size << endl;

  ThreadContext::Lock lock (context);

  /*
    get a block with at least size bytes
  */
  Block block = find_available (size);
  if (block->second < size)
  {
    Error error (InvalidState, "");
    error << "block size=" << block->second << " < required size=" << size;
    throw error;
  }

  char* ptr = block->first;

  /*
    cannot set block->first, so must create a new entry and delete the old
  */
  size_t remaining = block->second - size;
  if (remaining)
    add_available (ptr+size, remaining);

  available.erase (block);

  /*
    keep record of what has been allocated
  */
  add_allocated (ptr, size);

  return ptr;
}
 catch (Error& error)
   {
     throw error += "VirtualMemory::mmap";
   }

//! Free the memory to which the char* points
void VirtualMemory::munmap (void* ptr) try
{
  Block block = find_allocated( static_cast<char*>(ptr) );

  add_available (block->first, block->second);

  allocated.erase (block);
}
 catch (Error& error)
   {
     throw error += "VirtualMemory::munmap";
   }


//! Add allocated memory
void VirtualMemory::add_allocated (char* ptr, size_t size)
{
  allocated[ptr] = size;
}

//! Find allocated memory
VirtualMemory::Block VirtualMemory::find_allocated (char* ptr)
{
  for (Block block = allocated.begin(); block != allocated.end(); block++)
    if (block->first == ptr)
      return block;

  throw Error (InvalidState, "VirtualMemory::find_allocated",
	       "address=%s not in allocated list", ptr);
}

//! Find available memory
VirtualMemory::Block VirtualMemory::find_available (size_t size)
{
  for (Block block = available.begin(); block != available.end(); block++)
    if (block->second >= size)
      return block;
  
  return extend (size);
}

//! Add available memory
VirtualMemory::Block VirtualMemory::add_available (char* ptr, size_t size)
{
  char* end = ptr+size;

  Block block;

  /*
    check for contiguous blocks that are currently available
  */
  for (block = available.begin(); block != available.end(); block++)
  {
    char* block_end = block->first + block->second;

    if (block_end == ptr)
    {
      /*
	the new block follows an existing block, append to existing block
	and return the existing block
      */
      block->second += size;
      return block;
    }

    if (block->first == end)
    {
      /*
	an existing block follows the new block, erase the existing block
	and increase the size of the new block, then break
      */
      size += block->second;
      available.erase (block);
      break;
    }
  }

  // add a new block
  available[ptr] = size;
  return available.find (ptr);
}

VirtualMemory::Block VirtualMemory::extend (size_t size)
{
  size_t current = swap_space;

  do
  {
    if (swap_space == 0)
      swap_space = getpagesize();
    else
      swap_space *= 2;
  }
  while (swap_space < size);

  /*
    Stretch the file size to swap_space
  */
  int result = lseek( get_fd(), swap_space-1, SEEK_SET );

  if (result == -1)
  {
    close ();
    Error error (InvalidState, "VirtualMemory::extend");
    error << "could not lseek swap file to " << swap_space-1;
    throw error;
  }
    
  /*
    Write a null character at the end of the file
  */
  result = write( get_fd(), "", 1 );
  if (result != 1)
  {
    close ();
    Error error (InvalidState, "VirtualMemory::extend");
    error << "could not write end of swap file at " << swap_space;
    throw error;
  }

  /*
    Map the new swap space into memory
  */
  void* ptr = ::mmap( 0, swap_space-current,
		      PROT_READ | PROT_WRITE, MAP_SHARED,
		      get_fd(), current );

  if (ptr == MAP_FAILED)
  {
    close ();
    Error error (InvalidState, "VirtualMemory::extend");
    error << "could not mmap swap file from " << current
	  << " to " << swap_space;
    throw error;
  }

  try {
    return add_available( static_cast<char*>(ptr), swap_space-current );
  }
  catch (Error& error)
    {
      throw error += "VirtualMemory::extend";
    }
}

#if 0

/* Don't forget to free the mmapped memory
 */
if (munmap(map, FILESIZE) == -1) {
  perror("Error un-mmapping the file");
  /* Decide here whether to close(get_fd()) and exit() or not. Depends... */
 }

/* Un-mmaping doesn't close the file, so we still need to do that.
 */
close(get_fd());

#endif

