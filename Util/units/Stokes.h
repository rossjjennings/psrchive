//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Stokes.h,v $
   $Revision: 1.19 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __Stokes_H
#define __Stokes_H

#include "Vector.h"
#include "random.h"
#include "Error.h"

template <typename T>
class Stokes : public Vector<4,T>
{
 public:
  
  Stokes (T a=0.0, T b=0.0, T c=0.0, T d=0.0) : Vector<4,T> (a,b,c,d) {}

  //! Construct from a 4-vector
  template<typename U>
  Stokes (const Vector<4,U>& v) : Vector<4,T> (v[0], v[1], v[2], v[3]) {}

  //! Construct from a scalar and 3-vector
  template<typename U>
  Stokes (T s, const Vector<3,U>& v) : Vector<4,T> (s, v[0], v[1], v[2]) {}

  template<typename U>
  Stokes (const Stokes<U>& s) : Vector<4,T> (s[0], s[1], s[2], s[3]) {}

  //! Access to scalar component
  T get_scalar () const { return this->x[0]; }

  //! Set the scalar component
  void set_scalar (T s) { this->x[0] = s; }

  //! Access to vector component
  Vector<3,T> get_vector () const 
  { return Vector<3,T> (this->x[1], this->x[2], this->x[3]); }

  //! Set the vector component
  template<typename U>
  void set_vector (const Vector<3,U>& v) 
  { this->x[1]=v[0]; this->x[2]=v[1]; this->x[3]=v[2]; }

  T sqr_vect () const { return normsq (get_vector()); }

  T abs_vect () const { return sqrt (sqr_vect()); }

  T invariant () const { return this->x[0]*this->x[0] - sqr_vect(); }

};

// useful method for generating random source polarization
template <class T, class U>
void random_value (Stokes<T>& val, U scale, float max_polarization = 1.0)
{
  // total intensity is always equal to scale
  val[0] = scale;

  // generate a random fractional polarization, from 0 to 1
  T fraction_polarized;
  random_value (fraction_polarized, 0.5);
  fraction_polarized += 0.5;
  fraction_polarized *= max_polarization;

  unsigned i=0;
  
  for (i=1; i<4; i++)
    random_value (val[i], scale);

  T modp = val.abs_vect();

  scale *= fraction_polarized / modp;

  for (i=1; i<4; i++)
    val[i] *= scale;

  if (val.invariant() < -1e-10)
    throw Error (InvalidState, "random_value (Stokes)",
		 "det=%f<0", val.invariant());
}

template <class T, class U>
void random_vector (Stokes<T>& val, U scale)
{
  random_value (val, scale);
}

#endif
