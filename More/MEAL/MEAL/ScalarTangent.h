//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarTangent.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_ScalarTangent_H
#define __Calibration_ScalarTangent_H

#include "Calibration/UnaryScalar.h"

namespace Calibration {

  //! The tangent of a Scalar function
  class ScalarTangent : public UnaryScalar
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
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  };

}


#endif
