//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryScalar.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_UnaryScalar_H
#define __MEAL_UnaryScalar_H

#include "MEAL/UnaryRule.h"
#include "MEAL/Scalar.h"

namespace MEAL {

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
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the result and its gradient
    void calculate (double& result, std::vector<double>* gradient);

  };

}

#endif
