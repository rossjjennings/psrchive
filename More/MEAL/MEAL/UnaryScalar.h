//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/UnaryScalar.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_UnaryScalar_H
#define __Model_UnaryScalar_H

#include "MEPL/UnaryRule.h"
#include "MEPL/Scalar.h"

namespace Model {

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
    void calculate (double& result, vector<double>* gradient);

  };

}

#endif
