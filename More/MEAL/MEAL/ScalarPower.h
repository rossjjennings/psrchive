//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarPower.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_ScalarPower_H
#define __Model_ScalarPower_H

#include "MEPL/BinaryScalar.h"

namespace Model {

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
    string get_name () const;

  };

}


#endif
