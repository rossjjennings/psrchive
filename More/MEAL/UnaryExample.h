//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/UnaryExample.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

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
