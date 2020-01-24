/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Fixed.h"
#include "Pauli.h"

using namespace std;

// #define _DEBUG

#ifdef _DEBUG
#include <iostream>
#endif

Fixed::Fixed ()
{
  vertical = 0;
}

//! Get the name of the mount
std::string Fixed::get_name () const
{
  return "Fixed";
}

double Fixed::get_vertical () const
{
  build ();
  return vertical;
}

double Fixed::get_parallactic_angle () const
{
  return get_vertical ();
}

Jones<double> Fixed::get_response () const
{
  build ();
  return response;
}

void Fixed::build () const
{
  if (get_built())
    return;

  Mount::build ();

  for (unsigned ipol=0; ipol < 2; ipol++)
    for (unsigned jpol=0; jpol < 2; jpol++)
      response (ipol,jpol) = observatory_basis[ipol] * source_basis[jpol];

  /*
    compute the effective rotation about the line of sight
  */

  complex<double> det;
  Quaternion<double, Hermitian> herm;
  Quaternion<double, Unitary> unit;
  
  polar (det, herm, unit, response);

  double sin_phi = unit.get_vector()[2];
  vertical = asin (sin_phi);

#ifdef _DEBUG
  cerr << "Fixed::build vertical=" << vertical << endl;
#endif
}
