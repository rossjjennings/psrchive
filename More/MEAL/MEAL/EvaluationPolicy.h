//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/EvaluationPolicy.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
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
