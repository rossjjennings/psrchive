//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarBesselI0.h,v $
   $Revision: 1.1 $
   $Date: 2006/01/27 01:52:33 $
   $Author $ */
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
