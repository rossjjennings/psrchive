//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Scalar.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Calibration_Scalar_H
#define __Calibration_Scalar_H

#include <vector>
#include "Calibration/EvaluationBehaviour.h"

namespace Calibration {

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
    virtual double evaluate (vector<double>* grad=0) const = 0;

    //! Return the scalar value and its variance
    virtual void evaluate (Estimate<double>& value) const;

  };

}

#endif
