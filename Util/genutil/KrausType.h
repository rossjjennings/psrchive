//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/KrausType.h

#ifndef __KrausType_H
#define __KrausType_H

#include "Directional.h"

//! Kraus-type mounted antenna
/*!
  Where the flat primary reflects radio light towards the spherical
  secondary, which focuses it towards a mobile focal carriage. The
  primary tilts North-South to select any object near the meridian,
  while the focal carriage moves East-West along railroad ties to
  track objects near transit. - https://en.wikipedia.org/wiki/Kraus-type */

class KrausType : public Directional
{
public:

  //! Get the name of the mount
  std::string get_name () const;

  //! Get the vertical angle (rotation about the line of sight)
  double get_vertical () const;

protected:

  //! Get the receptor basis in the reference frame of the observatory
  Matrix<3,3,double> get_basis (const Vector<3,double>& from_source) const;

  //! Disable Directional::build
  void build () const;
};

#endif
