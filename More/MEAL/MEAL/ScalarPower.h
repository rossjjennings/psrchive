//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarPower.h

#ifndef __MEAL_ScalarPower_H
#define __MEAL_ScalarPower_H

#include "MEAL/BinaryScalar.h"

namespace MEAL {

  //! The tangent of a Scalar function
  class ScalarPower : public BinaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // BinaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return x^y
    double function (double arg1, double arg2) const;

    //! Return the partial derivative with respect to the base, x
    double partial_arg1 (double arg1, double arg2) const;

    //! Return the partial derivative with respect to the exponent, y
    double partial_arg2 (double arg1, double arg2) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  };

}


#endif
