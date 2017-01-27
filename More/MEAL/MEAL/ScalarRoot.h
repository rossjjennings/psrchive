//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarRoot.h

#ifndef __MEAL_ScalarRoot_H
#define __MEAL_ScalarRoot_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The square root of a Scalar function
  class ScalarRoot : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return sqrt(x)
    double function (double x) const;

    //! Return d/dx sqrt(x)
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
