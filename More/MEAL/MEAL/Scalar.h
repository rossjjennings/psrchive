//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Scalar.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_Scalar_H
#define __MEAL_Scalar_H

#include <vector>
#include "MEAL/EvaluationBehaviour.h"

namespace MEAL {

  //! Pure virtual base class of scalar functions
  /*! The Scalar class represents any scalar function with an
    arbitrary number of parameters. */
  class Scalar : public EvaluationBehaviour {

  public:

    //! The name of the class
    static const char* Name;

    //! The return type of the evaluate method
    typedef double Result;

    //! Return the scalar value (and its gradient, if requested)
    virtual double evaluate (std::vector<double>* grad=0) const = 0;

    //! Return the scalar value and its variance
    virtual void evaluate (Estimate<double>& value) const;

  };

}

#endif
