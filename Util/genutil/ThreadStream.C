/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadStream.h"
#include "tostring.h"

#include <fstream>

using namespace std;

#ifdef HAVE_PTHREAD
static void destructor (void* value)
{
  ostream* stream = reinterpret_cast<ostream*> (value);
  delete stream;
}
#endif

ThreadStream::ThreadStream ()
{
#ifdef HAVE_PTHREAD
  pthread_key_create (&key, &destructor);
#else
  stream = 0;
#endif
}

ThreadStream::~ThreadStream ()
{
#ifdef HAVE_PTHREAD
  pthread_key_delete (key);
#else
  if (stream) delete stream;
#endif
}

ostream& ThreadStream::get ()
{
#ifdef HAVE_PTHREAD
  ostream* stream = reinterpret_cast<ostream*>( pthread_getspecific (key) );

  if (!stream)
    throw Error (InvalidState, "ThreadStream::get",
		 "std::ostream not set for this thread");

#endif
  return *stream;
}

void ThreadStream::set (std::ostream* _stream)
{
#ifdef HAVE_PTHREAD
  pthread_setspecific (key, _stream);
#else
  stream = _stream;
#endif
}
