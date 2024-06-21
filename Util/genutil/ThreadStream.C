/***************************************************************************
 *
 *   Copyright (C) 2007 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadStream.h"
#include "tostring.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

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
  auto key = new pthread_key_t;
  pthread_key_create (key, &destructor);
  stream = key;
#else
  stream = 0;
#endif
}

ThreadStream::~ThreadStream ()
{
#ifdef HAVE_PTHREAD
  auto key = reinterpret_cast<pthread_key_t*>(stream);
  pthread_key_delete (*key);
  delete key;
#else
  if (stream) delete reinterpret_cast<ostream*> (stream);
#endif
}

ostream& ThreadStream::get ()
{
#ifdef HAVE_PTHREAD
  auto key = reinterpret_cast<pthread_key_t*>(stream);
  auto ostr = reinterpret_cast<ostream*>( pthread_getspecific (*key) );
#else
  auto ostr = reinterpret_cast<ostream*>(stream);
#endif

  if (!ostr)
    throw Error (InvalidState, "ThreadStream::get",
		 "std::ostream not set for this thread");
  return *ostr;
}

void ThreadStream::set (std::ostream* _stream)
{
#ifdef HAVE_PTHREAD
  auto key = reinterpret_cast<pthread_key_t*>(stream);
  pthread_setspecific (*key, _stream);
#else
  stream = _stream;
#endif
}
