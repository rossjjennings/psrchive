//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Estimate.h,v $
   $Revision: 1.3 $
   $Date: 2003/02/12 13:57:39 $
   $Author: straten $ */

#ifndef __Estimate_H
#define __Estimate_H

#include "operators.h"

//! Estimates have a value, \f$ x \f$, and a variance, \f$ \sigma_x^2 \f$
/*!
  Where \f$ y = f (x_1, x_2, ... x_n) \f$, then
  \f$ \sigma_y^2 = \sum_{i=1}^n ({\delta f \over \delta x_i })^2\sigma_i^2 \f$
*/
template <typename T>
class Estimate
{
 public:
  T val;
  T var;

  //! Construct from a value and its estimated error, \f$ \sigma^2 \f$
  Estimate (T _val=0, T _var=0) { val=_val; var=_var; }

  //! Construct from another Estimate
  Estimate (const Estimate& d) { val=d.val; var=d.var; }

  //! Assignment operator
  Estimate & operator= (const Estimate& d)
  { val=d.val; var=d.var; return *this; }

  Estimate & operator+= (const Estimate& d)
  { val += d.val; var += d.var; return *this; }

  Estimate & operator-= (const Estimate& d)
  { val -= d.val; var += d.var; return *this; }

  //! Multiply two estimates
  /*! Where \f$ r=x*y \f$, \f$ \sigma_r = y^2\sigma_x + x^2\sigma_y */
  Estimate & operator*= (const Estimate& d)
  { T v=val; val*=d.val; var=v*v*d.var+d.val*d.val*var; return *this; }

  //! Divide two estimates
  Estimate & operator/= (const Estimate& d)
  { return operator *= (d.inverse()); }

  //! Invert an estimate
  /*! Where \f$ r=1/x \f$, \f$ \sigma_r = r^2\sigma_x/x^2 = sigma_x/x^4 */
  const Estimate inverse () const
  { T v=1.0/val; return Estimate (v,var*v*v*v*v); }

};

#endif

#if 0

  T relerr() const;
  void set_relerr(T);

  bool consistent(const Estimate &d, T efac=1.0) const;

  // print in 2.31(2) form if places<0
  string print(int places=-1, char *sep=" ");

  Estimate & operator+=(const Estimate&);
  Estimate & operator-=(const Estimate&);
  Estimate & operator*=(const Estimate&);
  Estimate & operator/=(const Estimate&);

  Estimate & operator+=(T);
  Estimate & operator-=(T);
  Estimate & operator*=(T);
  Estimate & operator/=(T);

  friend Estimate  operator+(const Estimate&, const Estimate&);
  friend Estimate  operator-(const Estimate&, const Estimate&);
  friend Estimate  operator*(const Estimate&, const Estimate&);
  friend Estimate  operator/(const Estimate&, const Estimate&);

  friend Estimate  operator+(const Estimate&, T);
  friend Estimate  operator-(const Estimate&, T);
  friend Estimate  operator*(const Estimate&, T);
  friend Estimate  operator/(const Estimate&, T);

  friend Estimate  operator+(T b, const Estimate& a);
  friend Estimate  operator-(T b, const Estimate& a);
  friend Estimate  operator*(T b, const Estimate& a);
  friend Estimate  operator/(T b, const Estimate& a);

 protected:
  void copy(const Estimate&);
  void init() {val=err=0.0;}
};

 inline Estimate operator+(T b, const Estimate& a) {return a+b;}
 inline Estimate operator-(T b, const Estimate& a) {return a-b;}
 inline Estimate operator*(T b, const Estimate& a) {return a*b;}
 inline Estimate operator/(T b, const Estimate& a) {return a/b;}

}
#endif
