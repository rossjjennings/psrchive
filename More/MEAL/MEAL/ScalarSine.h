//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarSine.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_ScalarSine_H
#define __MEAL_ScalarSine_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The sine of a Scalar function
  class ScalarSine : public UnaryScalar
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
