//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BinaryScalar.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __MEAL_BinaryScalar_H
#define __MEAL_BinaryScalar_H

#include "MEAL/BinaryRule.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! Abstract base class of binary functions of two other Scalar functions
  /*! The partial derivatives are calculated using the chain rule. */
  class BinaryScalar : public BinaryRule<Scalar>
  {

  public:

    //! The function
    virtual double function (double arg1, double arg2) const = 0;

    //! The partial derivative with respect to arg1
    virtual double partial_arg1 (double arg1, double arg2) const = 0;

    //! The partial derivative with respect to arg2
    virtual double partial_arg2 (double arg1, double arg2) const = 0;

  protected:

    //! Return the result and its gradient
    void calculate (double& result, std::vector<double>* gradient);

  };

}

#endif
