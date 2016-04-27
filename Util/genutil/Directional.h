//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/Directional.h

#ifndef __Directional_H
#define __Directional_H

#include "Mount.h"

//! Directional antennae
class Directional : public Mount
{  
public:
  
  //! Default constructor
  Directional ();

  //! Get the vertical angle in radians (rotation about the line of sight)
  virtual double get_vertical () const;

  //! Synonym for vertical angle
  double get_parallactic_angle () const;

protected:

  //! Get the receptor basis in the reference frame of the observatory
  virtual Matrix<3,3,double>
  get_basis (const Vector<3,double>& from_source) const = 0;

  //! Lazily evaluate
  void build () const;

  //! The parallactic angle in radians
  mutable double vertical;
};

#endif
