//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarPower.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Calibration_ScalarPower_H
#define __Calibration_ScalarPower_H

#include "Calibration/BinaryScalar.h"

namespace Calibration {

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
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  };

}


#endif
