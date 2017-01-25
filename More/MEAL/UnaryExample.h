//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/UnaryExample.h

#ifndef __MEAL_UnaryExample_H
#define __MEAL_UnaryExample_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! Example of a UnaryScalar child
  class UnaryExample : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double x);

    //! And its first derivative
    double dfdx (double x);


    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  };

}


#endif
