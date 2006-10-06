//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_Complex2_H
#define __MEAL_Complex2_H

#include "MEAL/Function.h"
#include "MEAL/EvaluationPolicy.h"
#include "Estimate.h"
#include "Jones.h"

namespace MEAL {

  //! Pure virtual base class of all complex 2x2 matrix functions
  /*! The Complex2 class represents any complex 2x2 matrix function with an
    arbitrary number of parameters. */
  class Complex2 : public Function {

  public:

    //! The name of the class
    static const char* Name;

    //! The return type of the evaluate method
    typedef Jones<double> Result;

    //! Default constructor
    Complex2 ();

    //! Copy constructor
    Complex2 (const Complex2&);

    //! Assignment operator
    Complex2& operator = (const Complex2&);

    //! Return the Jones matrix and its gradient
    Result evaluate (std::vector<Result>* grad=0) const
    { return evaluation_policy->evaluate (grad); }

    //! Return the Jones Estimate matrix
    virtual void evaluate (Jones< Estimate<double> >& jones) const;

  protected:

    template<class T> friend class CalculatePolicy;

    //! The policy for managing function evaluation
    Reference::To< EvaluationPolicy< Jones<double> > > evaluation_policy;

    //! Calculate the Jones matrix and its gradient
    virtual void calculate (Jones<double>& result,
			    std::vector<Jones<double> >*) = 0;

    //! Use the calculate method of another Complex2 instance
    void calculate (Complex2* other, Jones<double>& result,
                    std::vector<Jones<double> >*);

  };

}

#endif
