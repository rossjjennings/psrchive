//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryScalar.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_UnaryScalar_H
#define __Calibration_UnaryScalar_H

#include "Calibration/UnaryRule.h"
#include "Calibration/Scalar.h"

namespace Calibration {

  //! Abstract base class of unary functions of another Scalar function
  /*! The partial derivatives are calculated using the chain rule. */
  class UnaryScalar : public UnaryRule<Scalar>
  {

  public:

    //! The function
    virtual double function (double x) const = 0;

    //! And its first derivative
    virtual double dfdx (double x) const = 0;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the result and its gradient
    void calculate (double& result, vector<double>* gradient);

  };

}

#endif
