//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarInverseTangent.h

#ifndef __MEAL_ScalarInverseTangent_H
#define __MEAL_ScalarInverseTangent_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The inverse tangent of the ratio of two Scalar functions
  class ScalarInverseTangent : public UnaryScalar
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
