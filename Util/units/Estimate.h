//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Estimate.h,v $
   $Revision: 1.17 $
   $Date: 2003/05/22 08:49:12 $
   $Author: straten $ */

#ifndef __Estimate_h
#define __Estimate_h

#include <iostream>
#include <math.h>
#include "psr_cpp.h"

//! Estimates with a value, \f$ x \f$, and a variance, \f$ \sigma^2 \f$
/*!
  Where \f$ y = f (x_1, x_2, ... x_n) \f$, then
  \f$ \sigma_y^2 = \sum_{i=1}^n ({\partial f \over \partial x_i})^2\sigma_i^2 \f$

  See http://mathworld.wolfram.com/ErrorPropagation.html
*/
template <typename T, typename U=T>
class Estimate
{
 public:
  //! Enables vector< Estimate<T> > to be used in fft::interpolate template
  static unsigned ndim;

  //! The value, \f$ x \f$
  T val;
  //! The variance of the value, \f$ \sigma_x^2 \f$
  U var;

  //! Construct from a value, \f$ x \f$, and its variance, \f$ \sigma_x^2 \f$
  Estimate (T _val=0, U _var=0) { val=_val; var=_var; }

  //! Construct from another Estimate
  template <typename V, typename W>
  Estimate (const Estimate<V,W>& d) { val=d.val; var=d.var; }

  //! Assignment operator
  const Estimate& operator= (const Estimate& d)
  { val=d.val; var=d.var; return *this; }

  //! Array access to value
  T& operator [] (unsigned n)
  { return val; }

  //! Array access to value
  T operator [] (unsigned n) const
  { return val; }

  //! Addition operator
  const Estimate& operator+= (const Estimate& d)
  { val += d.val; var += d.var; return *this; }

  //! Subtraction operator
  const Estimate& operator-= (const Estimate& d)
  { val -= d.val; var += d.var; return *this; }

  //! Multiplication operator
  /*! Where \f$ r=x*y \f$, \f$ \sigma_r = y^2\sigma_x + x^2\sigma_y */
  const Estimate& operator*= (const Estimate& d)
  { T v=val; val*=d.val; var=v*v*d.var+d.val*d.val*var; return *this; }

  //! Division operator
  const Estimate& operator/= (const Estimate& d)
  { return operator *= (d.inverse()); }

  //! Equality operator
  bool operator == (const Estimate& d) const
  { return val == d.val; }

  //! Inequality operator
  bool operator != (const Estimate& d) const
  { return ! operator == (d); }

  //! Comparison operator
  bool operator < (const Estimate& d) const
  { return val < d.val; }

  //! Comparison operator
  bool operator > (const Estimate& d) const
  { return val > d.val; }

  //! Inversion operator
  /*! Where \f$ r=1/x \f$, \f$ \sigma_r = r^2\sigma_x/x^2 = sigma_x/x^4 */
  const Estimate inverse () const
  { T v=1.0/val; return Estimate (v,var*v*v*v*v); }

  friend const Estimate operator + (Estimate a, const Estimate& b)
  { return a+=b; }

  friend const Estimate operator - (Estimate a, const Estimate& b)
  { return a-=b; }
  
  friend const Estimate operator * (Estimate a, const Estimate& b)
  { return a*=b; }
  
  friend const Estimate operator / (Estimate a, const Estimate& b)
  { return a/=b; }

  //! Negation operator
  friend const Estimate operator - (Estimate a)
  { return Estimate (-a.val, a.var); }

  //! See http://mathworld.wolfram.com/ErrorPropagation.html Equation (15)
  friend const Estimate exp (const Estimate& u)
  { T val = std::exp (u.val); return Estimate (val, val*val*u.var); }

  //! See http://mathworld.wolfram.com/ErrorPropagation.html Equation (17)
  friend const Estimate log (const Estimate& u)
  { return Estimate (std::log (u.val), u.var/(u.val*u.val)); }

  //! \f$ \left({\partial\sin x\over\partial x}\right)^2 = (1-\sin^2x) \f$
  friend const Estimate sin (const Estimate& u)
  { T val = std::sin (u.val); return Estimate (val, (1-val*val)*u.var); }

  //! \f$ \left({\partial\cos x\over\partial x}\right)^2 = (1-\cos^2x) \f$
  friend const Estimate cos (const Estimate& u)
  { T val = std::cos (u.val); return Estimate (val, (1-val*val)*u.var); }

  //! \f$ {\partial\over\partial x} \tan^-1 (x) = (1+x^2)^{-1} \f$
  friend const Estimate atan2 (const Estimate& s, const Estimate& c)
  { T c2 = c.val*c.val;  T s2 = s.val*s.val;
    return Estimate (std::atan2 (s.val, c.val), (c2*s.var+s2*c.var)/(c2+s2)); }
  
  //! \f$ {\partial\over\partial x} x^\onehalf = \onehalf x^{-\onehalf} \f$
  friend const Estimate sqrt (const Estimate& u)
  { return Estimate (std::sqrt (u.val), 0.25*u.var/fabs(u.val)); }

};


template <typename T, typename U>
unsigned Estimate<T, U>::ndim = 1;


//! Useful for quickly printing the values
template<typename T, typename U>
ostream& operator<< (ostream& ostr, const Estimate<T,U>& estimate)
{
  return ostr << "(" << estimate.val << "\261" << estimate.var << ")";
}


/*!
  \f$ {\bar{x} over \bar{\sigma}^2} = \sum_{i=1}^n {x_i \over \sigma_i^2} \f$

  See http://mathworld.wolfram.com/MaximumLikelihood.html (Eqs. 16 and 19)
*/
template <typename T, typename U=T>
class MeanEstimate
{
 public:
  //! The value, normalized by its variance
  T norm_val;
  //! The inverse of its variance
  U inv_var;

  //! Construct from a value (normalized by / and) the inverse of its variance
  MeanEstimate (T _val=0, U _var=0) { norm_val=_val; inv_var=_var; }

  //! Construct from another MeanEstimate
  MeanEstimate (const MeanEstimate& d)
  { norm_val=d.norm_val; inv_var=d.inv_var; }

  //! Assignment operator
  const MeanEstimate& operator= (const MeanEstimate& d)
  { norm_val=d.norm_val; inv_var=d.inv_var; return *this; }

  //! Addition operator
  const MeanEstimate& operator+= (const MeanEstimate& d)
  { norm_val += d.norm_val; inv_var += d.inv_var; return *this; }

  //! Addition operator
  const MeanEstimate& operator+= (const Estimate<T,U>& d)
  { if (d.var){ U v=1.0/d.var; norm_val+=d.val*v; inv_var+=v; } return *this; }

  //! Equality operator
  bool operator == (T _norm_val) const
  { return norm_val == _norm_val; }

  Estimate<T,U> get_Estimate () const
  { U var=0.0; if (inv_var != 0.0) var = 1.0/inv_var; 
    return Estimate<T,U> (norm_val*var, var); }

};

template <typename T, typename U=T>
class MeanRadian
{

 public:
  //! Addition operator
  const MeanRadian& operator+= (const MeanRadian& d)
  { cosine += d.cosine; sine += d.sine; return *this; }

  //! Addition operator, Estimate represents a value in radians
  const MeanRadian& operator+= (const Estimate<T,U>& d)
  { cosine += cos(d); sine += sin(d); return *this; }

  Estimate<T,U> get_Estimate () const
  { return atan2 (sine.get_Estimate(), cosine.get_Estimate()); }
  
 protected:
  //! The average cosine
  MeanEstimate<T,U> cosine;

  //! The average sine
  MeanEstimate<T,U> sine;

};

#endif

