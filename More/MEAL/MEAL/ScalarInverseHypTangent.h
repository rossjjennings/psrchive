//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarInverseHypTangent.h,v $
   $Revision: 1.4 $
   $Date: 2004/11/23 12:14:35 $
   $Author: straten $ */

#ifndef __MEAL_ScalarInverseHypTangent_H
#define __MEAL_ScalarInverseHypTangent_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The inverse hyperbolic tangent of a Scalar function
  class ScalarInverseHypTangent : public UnaryScalar
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
