//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __MEAL_Complex2_H
#define __MEAL_Complex2_H

#include "MEAL/EvaluationBehaviour.h"
#include "Estimate.h"
#include "Jones.h"

namespace MEAL {

  //! Pure virtual base class of all complex 2x2 matrix functions
  /*! The Complex2 class represents any complex 2x2 matrix function with an
    arbitrary number of parameters. */
  class Complex2 : public EvaluationBehaviour {

  public:

    //! The name of the class
    static const char* Name;

    //! The return type of the evaluate method
    typedef Jones<double> Result;

    //! Return the Jones matrix and its gradient wrt model parameters
    virtual Result evaluate (std::vector<Result>* grad=0) const = 0;

    //! Return the Jones Estimate matrix
    virtual void evaluate (Jones< Estimate<double> >& jones) const;

  };

}

#endif
