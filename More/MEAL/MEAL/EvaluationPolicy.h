//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/EvaluationPolicy.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:24:36 $
   $Author: straten $ */

#ifndef __MEAL_EvaluationPolicy_H
#define __MEAL_EvaluationPolicy_H

#include "MEAL/FunctionPolicy.h"
#include <vector>

namespace MEAL {

  //! Abstract base class of Function parameter policies
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
