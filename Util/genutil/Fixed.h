//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/Fixed.h

#ifndef __Fixed_H
#define __Fixed_H

#include "MountProjection.h"

//! Fixed antennae
class Fixed : public MountProjection
{  
public:
  
  //! Default constructor
  Fixed ();

  //! Get the name of the mount
  std::string get_name () const;

  //! Get the vertical angle in radians (rotation about the line of sight)
  double get_vertical () const;

  //! Synonym for vertical angle
  double get_parallactic_angle () const;

  //! Get the response
  Jones<double> get_response () const;

protected:

  //! Lazily evaluate
  void build () const;

  //! The parallactic angle in radians
  mutable double vertical;

  //! The Jones matrix that describes the projection of the receptors
  mutable Jones<double> response;
};

#endif

