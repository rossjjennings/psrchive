//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/units/complex_public.h

#ifndef __complex_public
#define __complex_public

#include <complex>

//! simple template used to provide public access to std::complex::real|imag
template<class T>
class complex_public {
public:
  T real;
  T imag;
};

template<class T>
complex_public<T>& pub (std::complex<T>& c)
{
  return *reinterpret_cast< complex_public<T>* >( &c );
}

template<class T>
const complex_public<T>& pub (const std::complex<T>& c)
{
  return *reinterpret_cast< const complex_public<T>* >( &c );
}

#endif
