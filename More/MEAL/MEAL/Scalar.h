//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Scalar.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_Scalar_H
#define __MEAL_Scalar_H

#include "MEAL/Function.h"
#include "MEAL/EvaluationPolicy.h"

namespace MEAL {

  //! Pure virtual base class of scalar functions
  /*! The Scalar class represents any scalar function with an
    arbitrary number of parameters. */
  class Scalar : public Function {

  public:

    //! The name of the class
    static const char* Name;

    //! The return type of the evaluate method
    typedef double Result;

    //! Default constructor
    Scalar ();

    //! Copy constructor
    Scalar (const Scalar&);

    //! Assignment operator
    Scalar& operator = (const Scalar&);

    //! Return the scalar value (and its gradient, if requested)
    double evaluate (std::vector<double>* grad=0) const
    { return evaluation_policy->evaluate (grad); }

    //! Return the scalar value and its variance
    virtual void evaluate (Estimate<double>& value) const;

  protected:

    template<class T> friend class CalculatePolicy;

    //! The policy for managing function evaluation
    Reference::To< EvaluationPolicy<Result> > evaluation_policy;

    //! Calculate the scalar value and its gradient
    virtual void calculate (double& result, std::vector<double>*) = 0;

    //! Copy the parameter policy of another instance
    void copy_evaluation_policy (const Scalar*);

  };

}

#endif
