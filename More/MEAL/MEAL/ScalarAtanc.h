//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarAtanc.h,v $
   $Revision: 1.1 $
   $Date: 2009/06/11 06:39:43 $
   $Author: straten $ */

#ifndef __MEAL_ScalarAtanc_H
#define __MEAL_ScalarAtanc_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! Like sinc(x), atanc(x) = \f$ \tan^{-1}(\pi x / 2) \over \pi x / 2\f$
  class ScalarAtanc : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double x) const;

    //! And its first derivative
    double dfdx (double x) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  };


}

// returns atan(x*pi/2)/(x*pi/2)
double atanc (double x, double* derivative = 0, double* datancc = 0);

#endif
