//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Jacobi.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/22 15:42:38 $
   $Author: straten $ */

#ifndef __Jacobi_H
#define __Jacobi_H

#include "Matrix.h"

template <typename T, unsigned RC>
static inline void rotate (Matrix<T,RC,RC>& x, T s, T tau,
			   unsigned i, unsigned j, unsigned k, unsigned l)
{
  T g=x[i][j];
  T h=x[k][l];

  x[i][j]=g-s*(h+g*tau);
  x[k][l]=h+s*(g-h*tau);
}

// returns the correction factor for the diagonal
template <typename T, typename U, unsigned RC>
U JacobiRotation ( unsigned ip, unsigned iq, 
		   Matrix<T,RC,RC>& a, Matrix<T,RC,RC>& v, Vector<U,RC>& d )
{
  U h = d[iq] - d[ip];
  U t, theta;
  
  U g = 100.0 * norm(a[ip][iq]);

  if (norm(h)+g == norm(h))
    t=(a[ip][iq])/h;
  else {
    theta=0.5*h/(a[ip][iq]);
    t=1.0/(norm(theta)+sqrt(1.0+theta*theta));
    if (theta < 0.0) t = -t;
  }
  
  U c=1.0/sqrt(1+t*t);
  U s=t*c;
  U tau=s/(1.0+c);
  U correction = t*a[ip][iq];

  d[ip] -= correction;
  d[iq] += correction;
  
  a[ip][iq]=0.0;

  unsigned j;

  for (j=0; j<ip; j++)
    rotate(a,s,tau,j,ip,j,iq);

  for (j=ip+1; j<iq; j++)
    rotate(a,s,tau,ip,j,j,iq);

  for (j=iq+1; j<RC; j++)
    rotate(a,s,tau,ip,j,iq,j);

  for (j=0; j<RC; j++)
    rotate(v,s,tau,j,ip,j,iq);

  return correction;
}


template <typename T>
T real (T x) { return x; }

template <typename T>
T norm (T x) { return fabs(x); }

template <typename T, typename U, unsigned RC>
void Jacobi (Matrix<T,RC,RC>& a, Matrix<T,RC,RC>& evec, Vector<U,RC>& eval)
{
  Vector<U,RC> b, z;

  // copy the diagonal elements into d and b
  unsigned ip, iq;
  for (ip=0; ip<RC; ip++)
    b[ip] = eval[ip] = real( a[ip][ip] );

  // start with the identity matrix
  identity (evec);

  for (unsigned iter=0; iter < 50; iter++) {

    // sum off diagonal elements
    U sum = 0.0;

    for (ip=0; ip < RC; ip++)
      for (iq=ip+1; iq < RC; iq++)
	sum += norm(a[ip][iq]);

    if (sum == 0.0)
      return;

    U thresh = 0;

    if (iter < 4)
      thresh=0.2*sum/(RC*RC);

    for (ip=0; ip < RC; ip++)
      for (iq=ip+1; iq < RC; iq++) {

	// after four iterations, skip the rotation if the off-diagonal
	// element is small

	U g = 100.0 * norm(a[ip][iq]);

	if ( iter > 4 
	     && norm(eval[ip])+g == norm(eval[ip])
	     && norm(eval[iq])+g == norm(eval[iq]) )

	  a[ip][iq]=0.0;


	else if (norm(a[ip][iq]) > thresh) {

	  U correction = JacobiRotation ( ip, iq, a, evec, eval );

	  z[ip] -= correction;
	  z[iq] += correction;

	}
      }

    for (ip=0; ip < RC; ip++) {
      b[ip] += z[ip];
      eval[ip]=b[ip];
      z[ip]=0.0;
    }


  } // for each iteration
  
}

#endif
