//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Traits.h,v $
   $Revision: 1.1 $
   $Date: 2004/10/26 12:45:43 $
   $Author: straten $ */

/*
 * For a useful discussion on traits, please see 
 *
 * "Traits: a new and useful template technique" by Nathan C. Meyers
 * C++ Report, June 1995 http://www.cantrip.org/traits.html
 *
 * These traits were implemented in order that data types such as Jones
 * matrices and Quaternions can be passed to algorithms designed to work
 * with scalar values like float and double.
 *
 * Willem van Straten - 26 October 2004
 *
 */

#ifndef __Traits_h
#define __Traits_h

#include <complex>

//! Traits of an element type
template< class E > struct ElementTraits
{
  //! How to cast a complex type to the element type
  template< class T >
  static inline E cast (const std::complex<T>& value)
  { return value.real(); }
};

//! Partial specialization for complex elements
template< class E > struct ElementTraits< std::complex<E> >
{
  //! How to cast a complex type to the complex element type
  template< class T >
  static inline std::complex<E> cast (const std::complex<T>& value)
  { return value; }
};

//! Traits of the data type
template< class T, class E = ElementTraits<T> > struct DatumTraits
{
  //! Traits of the elements of the data type
  E element_traits;

  static inline unsigned ndim () { return 1; }
  static inline T& element (T& t, unsigned idim) { return t; }
  static inline const T& element (const T& t, unsigned idim) { return t; }
};

#endif
