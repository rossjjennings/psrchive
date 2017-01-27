//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarBesselI0.h

#ifndef __MEAL_ScalarBesselI0_H
#define __MEAL_ScalarBesselI0_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! Modified Bessel Function of the first kind, order zero
  class ScalarBesselI0 : public UnaryScalar
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


#endif
