//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/EvaluationPolicy.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/12 13:35:02 $
   $Author: straten $ */

#ifndef __MEAL_EvaluationPolicy_H
#define __MEAL_EvaluationPolicy_H

#include "MEAL/FunctionPolicy.h"
#include <vector>

namespace MEAL {

  //! Managers of Function value and gradient
  template<class Result>
  class EvaluationPolicy : public FunctionPolicy {

  public:

    //! Default destructor
    EvaluationPolicy (Function* context) : FunctionPolicy (context) { }

    //! Return the result (and its gradient, if requested)
    virtual Result evaluate (std::vector<Result>* grad=0) const = 0;

  };

}

#endif
