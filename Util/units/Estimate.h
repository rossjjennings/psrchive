//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Estimate.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/10 19:20:42 $
   $Author: straten $ */

#ifndef __Estimate_H
#define __Estimate_H

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
