//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/ThreadStream.h

#ifndef __ThreadStream_h
#define __ThreadStream_h

#include <iostream>

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

  //! pointer to either a pthread_key_t (HAVE_PTHREAD) or a std::ostream (!HAVE_PTHREAD)
  void* stream = nullptr;
};

template<class T>
ThreadStream& operator<< (ThreadStream& ts, const T& t)
{
  ts.get() << t;
  return ts;
}

typedef std::ostream& (*manipulator) ( std::ostream& os );

//! Handle manipulators
inline ThreadStream& operator<< (ThreadStream& ts, manipulator m)
{
  ts.get() << m;
  return ts;
}

#endif
