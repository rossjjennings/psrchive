/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "KrausType.h"

//! Get the name of the mount
std::string KrausType::get_name () const
{
  return "Kraus-type";
}

/*! sin(PA) = sin(dec) * sin(HA) */
double KrausType::get_vertical () const
{
  double HA = get_hour_angle();
  return asin( sin(declination) * sin(HA) );
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> KrausType::get_basis (const Vector<3,double>&) const
{
  return Matrix<3,3,double>(0.0);
}

void KrausType::build () const
{
  Mount::build ();
}
