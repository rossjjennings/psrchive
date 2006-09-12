//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Real4.h,v $
   $Revision: 1.1 $
   $Date: 2006/09/12 08:03:40 $
   $Author: straten $ */

#ifndef __MEAL_Real4_H
#define __MEAL_Real4_H

#include "MEAL/Function.h"
#include "MEAL/EvaluationPolicy.h"
#include "Matrix.h"

namespace MEAL {

  //! Pure virtual base class of all real-valued 4x4 matrix functions
  class Real4 : public Function {

  public:

    //! The name of the class
    static const char* Name;

    //! The return type of the evaluate method
    typedef Matrix<4,4,double> Result;

    //! Default constructor
    Real4 ();

    //! Copy constructor
    Real4 (const Real4&);

    //! Assignment operator
    Real4& operator = (const Real4&);

    //! Return the Jones matrix and its gradient
    Result evaluate (std::vector<Result>* grad=0) const
    { return evaluation_policy->evaluate (grad); }

  protected:

    template<class T> friend class CalculatePolicy;

    //! The policy for managing function evaluation
    Reference::To< EvaluationPolicy< Matrix<4,4,double> > > evaluation_policy;

    //! Calculate the Jones matrix and its gradient
    virtual void calculate (Matrix<4,4,double>& result,
			    std::vector<Matrix<4,4,double> >*) = 0;

  };

}

#endif
