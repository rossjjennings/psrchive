//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Math.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Calibration_Complex2Math_H
#define __Calibration_Complex2Math_H

#include "Calibration/Complex2.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<Calibration::Complex2>
operator + (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b);

//! Return a reference to a new SumRule instance representing a-b
Reference::To<Calibration::Complex2>
operator - (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<Calibration::Complex2>
operator * (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b);

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<Calibration::Complex2>
operator / (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b);

#endif

