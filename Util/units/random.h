//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/random.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __random_H
#define __random_H

#include <complex>
#include <stdlib.h>

template <class T, class U>
void random_value (T& value, U scale)
{
  value = ( double(rand()) - 0.5*double(RAND_MAX) ) * 2.0 * scale / RAND_MAX;
}

template <class T, class U>
void random_value (std::complex<T>& value, U scale)
{
  T real=0, imag=0;

  random_value (real, scale);
  random_value (imag, scale);

  value = std::complex<T> (real, imag);
}

template <class T, class U>
void random_vector (T& array, U scale)
{
  for (unsigned i=0; i<array.size(); i++)
    random_value (array[i], scale);
}

template <class T, class U>
void random_matrix (T& array, U scale)
{
  for (unsigned i=0; i<array.size(); i++)
    random_vector (array[i], scale);
}

#endif
