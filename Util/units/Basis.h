//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Basis.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/06 11:46:52 $
   $Author: straten $ */

#ifndef __Basis_H
#define __Basis_H

#include "Matrix.h"
#include "Types.h"

//! Defines the basis in which the electric field is represented
template<typename T>
class Basis {

public:

  //! set basis to circular or linear
  void set_basis (Signal::Basis basis);

  //! set basis to elliptical
  void set_basis (double orientation, double ellipticity);

  //! get the orientation
  double get_orientation () const { return orientation; }

  //! get the ellipticity
  double get_ellipticity () const { return ellipticity; }

  //! get the basis vector
  Vector<T, 3> get_basis_vector (unsigned iaxis) const { return xform[iaxis]; }

  //! convert the input vector to the basis
  template<typename U>
  Vector<U,3> Basis::convert (Vector<U,3>& vect) { return xform * vect; }

protected:

  //! The basis code
  Signal::Basis basis;

  //! The orientation 
  double orientation;

  //! The ellipticity
  double ellipticity;

  //! The basis matrix
  Matrix<T, 3, 3> xform;

};

/*! Given the orientation and ellipticity in radians, calculate the basis
  vectors used to convert Stokes parameters to coherency matrix. */
template<typename T>
void Basis<T>::set_basis (double _orientation, double _ellipticity)
{
  orientation = _orientation;
  ellipticity = _ellipticity;

  double cos_2o = cos (2.0*orientation);
  double sin_2o = sin (2.0*orientation);
  double cos_2e = cos (2.0*ellipticity);
  double sin_2e = sin (2.0*ellipticity);

  xform[0] = Vector<T,3> (cos_2o*cos_2e, -sin_2o*cos_2e, sin_2e);
  xform[1] = Vector<T,3> (sin_2o, cos_2o, 0);
  xform[2] = Vector<T,3> (-cos_2o*sin_2e, sin_2o*sin_2e, cos_2e);

  basis = Signal::Elliptical;
}

//! set basis to circular or linear
template<typename T>
void Basis<T>::set_basis (Signal::Basis _basis)
{
  basis = _basis;

  switch (basis)  {

  case Signal::Linear:
    xform[0] = Vector<T, 3>::basis (0);
    xform[1] = Vector<T, 3>::basis (1);
    xform[2] = Vector<T, 3>::basis (2);
    orientation = 0;
    ellipticity = 0;
    break;

  case Signal::Circular:
    xform[0] = Vector<T, 3>::basis (2);
    xform[1] = Vector<T, 3>::basis (0);
    xform[2] = Vector<T, 3>::basis (1);
    orientation = 0.25*M_PI;
    ellipticity = 0.25*M_PI;
    break;

  default:
    throw Error (InvalidParam, "Basis::set_basis unrecognized basis");

  }
}

#endif

