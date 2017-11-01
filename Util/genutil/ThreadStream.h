//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/ThreadStream.h

#ifndef __ThreadStream_h
#define __ThreadStream_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_PTHREAD
#include <pthread.h>
#endif

#include "Warning.h"

//! Manages a unique std::ostream for each thread
class ThreadStream
{
public:

  //! Default constructor
  ThreadStream ();

  //! Destructor
  ~ThreadStream ();

  //! Interface to ostream
  std::ostream& get ();

  //! Interface to raw void*
  void * get_raw ();

  //! Set the ostream for the current thread
  void set (std::ostream*);

  //! Set the raw void* for the current thread
  void set (void *);

private:

#ifdef HAVE_PTHREAD
  pthread_key_t key;
#else
  std::ostream* stream;
#endif

};

template<class T>
ThreadStream& operator<< (ThreadStream& ts, const T& t)
{
  ts.get() << t;
  return ts;
}

//! Handle manipulators
inline ThreadStream& operator<< (ThreadStream& ts, manipulator m)
{
  ts.get() << m;
  return ts;
}

#endif
