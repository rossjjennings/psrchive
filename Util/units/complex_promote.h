//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/complex_promote.h,v $
   $Revision: 1.3 $
   $Date: 2005/01/30 09:08:11 $
   $Author: straten $ */

#ifndef __complex_promote_H
#define __complex_promote_H

#include <complex>
#include "PromoteTraits.h"

#if defined(__ICC) || ( defined(__GNUC__) && __GNUC__ < 3 )

template <class T, class U>
class PromoteTraits< std::complex<T>, std::complex<U> >
{
  public:
    typedef std::complex<typename PromoteTraits<T,U>::promote_type> 
            promote_type;
};

#endif

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

