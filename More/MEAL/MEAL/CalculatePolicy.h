//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CalculatePolicy.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:24:36 $
   $Author: straten $ */

#ifndef __MEAL_CalculatePolicy_H
#define __MEAL_CalculatePolicy_H

#include "MEAL/EvaluationPolicy.h"

namespace MEAL {

  //! Abstract base class of Function parameter policies
  template<class T>
  class CalculatePolicy : public EvaluationPolicy<typename T::Result> {

    typedef typename T::Result Result;

  public:

    //! Default destructor
    CalculatePolicy (T* context) : EvaluationPolicy<Result> (context) 
    { calculate_context = context; }

    void calculate (Result& r, std::vector<Result>* grad) const
    { calculate_context->calculate (r, grad); }

  protected:

    //! The object that implements a calculate method
    T* calculate_context;

  };

}

#endif
