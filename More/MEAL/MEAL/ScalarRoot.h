//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarRoot.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_ScalarRoot_H
#define __MEAL_ScalarRoot_H

#include "MEAL/UnaryScalar.h"

namespace MEAL {

  //! The sine of a Scalar function
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
