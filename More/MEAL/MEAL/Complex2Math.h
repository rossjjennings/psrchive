//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Math.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:08 $
   $Author: straten $ */

#ifndef __Model_Complex2Math_H
#define __Model_Complex2Math_H

#include "MEPL/Complex2.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<Model::Complex2>
operator + (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b);

//! Return a reference to a new SumRule instance representing a-b
Reference::To<Model::Complex2>
operator - (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<Model::Complex2>
operator * (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<Model::Complex2>
operator / (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b);

#endif

