//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/BinaryScalar.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:08 $
   $Author: straten $ */

#ifndef __Model_BinaryScalar_H
#define __Model_BinaryScalar_H

#include "MEPL/BinaryRule.h"
#include "MEPL/Scalar.h"

namespace Model {

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
