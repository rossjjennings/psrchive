//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/complex_promote.h,v $
   $Revision: 1.1 $
   $Date: 2004/12/21 21:23:52 $
   $Author: straten $ */

#ifndef __complex_promote_H
#define __complex_promote_H

#include <complex>
#include "PromoteTraits.h"

//! std::complex addition
template<typename T, typename U>
const std::complex<typename PromoteTraits<T,U>::promote_type>
operator + (const std::complex<T>& a, const std::complex<U>& b)
{
  std::complex<typename PromoteTraits<T,U>::promote_type> ret(a);
  ret+=b;
  return ret;
}

//! std::complex subtraction
template<typename T, typename U>
const std::complex<typename PromoteTraits<T,U>::promote_type>
operator - (const std::complex<T>& a, const std::complex<U>& b)
{
  std::complex<typename PromoteTraits<T,U>::promote_type> ret(a);
  ret-=b;
  return ret;
}

//! std::complex multiplication
template<typename T, typename U>
const std::complex<typename PromoteTraits<T,U>::promote_type>
operator * (const std::complex<T>& a, const std::complex<U>& b)
{
  std::complex<typename PromoteTraits<T,U>::promote_type> ret(a);
  ret*=b;
  return ret;
}

#endif

