/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadMemory.h"
#include "tostring.h"

#include <stdlib.h> 

using namespace std;

static void destructor (void* memory)
{
  if (memory)
    free (memory);
}


ThreadMemory::ThreadMemory ()
{
#ifdef HAVE_PTHREAD
  pthread_key_create (&key, &destructor);
#else
  memory = 0;
#endif
}

ThreadMemory::~ThreadMemory ()
{
#ifdef HAVE_PTHREAD
  pthread_key_delete (key);
#else
  destructor (memory);
#endif
}

void * ThreadMemory::get ()
{
#ifdef HAVE_PTHREAD
  void * memory= pthread_getspecific (key);
  if (!memory)
    throw Error (InvalidState, "ThreadMemory::get",
		 "key ptr not set for this thread");
#endif
  return memory;
}

void ThreadMemory::set (void* _memory)
{
#ifdef HAVE_PTHREAD
  pthread_setspecific (key, _memory);
#else
  memory = _memory;
#endif
}

