//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Brent.h,v $
   $Revision: 1.1 $
   $Date: 2006/04/23 14:59:03 $
   $Author: straten $ */

#ifndef __BrentMethod
#define __BrentMethod

#include "Error.h"
#include <algorithm>
#include <math.h>

#define EPS 3.0e-8

template<class T, class U>
  T sign (T a, U b)
{
  if (b >= 0)
    return fabs(a);
  else
    return -fabs(a);
}

static unsigned iter_max = 100;

template<typename T, class Unary>
  T Brent (Unary func, T x1, T x2, T tol, T root = 0)
{
  T a = x1;
  T b = x2;
  T c = x2;
    
  T fa = func(a) - root;
  T fb = func(b) - root;

  if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0)) {
    Error error (InvalidParam, "Brent", "Root must be bracketed:");
    error << " f(" << x1 << ")=" << fa << " f(" << x2 << ")=" << fb;
    throw error;
  }

  T fc = fb;

  for (unsigned iter=0; iter<iter_max; iter++) {

    T d, e;

    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
      c=a;
      fc=fa;
      e=d=b-a;
    }
    if (fabs(fc) < fabs(fb)) {
      a=b;
      b=c;
      c=a;
      fa=fb;
      fb=fc;
      fc=fa;
    }
    T tol1 = 2.0*EPS*fabs(b) + 0.5*tol;
    T xm = 0.5*(c-b);
    if (fabs(xm) <= tol1 || fb == 0.0) {
      // std::cerr << "iter=" << iter << std::endl;
      return b;
    }
    if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) {
      T p, q, r, s=fb/fa;
      if (a == c) {
	p=2.0*xm*s;
	q=1.0-s;
      } else {
	q=fa/fc;
	r=fb/fc;
	p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
	q=(q-1.0)*(r-1.0)*(s-1.0);
      }

      if (p > 0.0) q = -q;
      p=fabs(p);
      T min1=3.0*xm*q-fabs(tol1*q);
      T min2=fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2)) {
	e=d;
	d=p/q;
      } else {
	d=xm;
	e=d;
      }
    } else {
      d=xm;
      e=d;
    }
    a=b;
    fa=fb;
    if (fabs(d) > tol1)
      b += d;
    else
      b += sign(tol1,xm);
    fb = func(b) - root;
  }

  throw Error (InvalidState, "Brent", "maximum iterations exceeded");
}

#endif
