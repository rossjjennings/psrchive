//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/OwnStream.h,v $
   $Revision: 1.1 $
   $Date: 2009/06/07 00:01:35 $
   $Author: straten $ */

#ifndef __OwnStream_h
#define __OwnStream_h

#include "ReferenceAble.h"
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

  //! Set cout
  void set_cout (std::ostream&) const;

  //! Set cerr
  void set_cerr (std::ostream&) const;

 protected:

  mutable std::ostream cout;
  mutable std::ostream cerr;
};

#endif
