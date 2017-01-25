//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/OwnStream.h

#ifndef __OwnStream_h
#define __OwnStream_h

#include "Reference.h"
#include <ostream>

//! Redefines cerr and cout as attributes of the class
/*! Useful in multi-threaded applications: by simply inheriting this class,
  cerr and cout can be redirected to different files for different threads,
  or on an instance-by-instance basis. */

class OwnStream : public Reference::Able
{
 public:

  //! Default constructor
  OwnStream ();

  //! Copy constructor
  OwnStream (const OwnStream&);

  //! Assignment operator
  const OwnStream& operator = (const OwnStream&);

  //! Set cout
  virtual void set_cout (std::ostream&) const;

  //! Set cerr
  virtual void set_cerr (std::ostream&) const;

 protected:

  mutable std::ostream cout;
  mutable std::ostream cerr;
};

#endif
