//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ArgumentPolicy.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:30:56 $
   $Author: straten $ */

#ifndef __MEAL_ArgumentPolicy_H
#define __MEAL_ArgumentPolicy_H

#include "MEAL/FunctionPolicy.h"

namespace MEAL {

  class Argument;

  //! Abstract base class of Function argument policies
  class ArgumentPolicy : public FunctionPolicy {

  public:

    //! Default constructor
    ArgumentPolicy (Function* context);

    //! Clone operator 
    virtual ArgumentPolicy* clone (Function* context) const = 0;

    //! Set the independent variable of the specified dimension
    virtual void set_argument (unsigned dimension, Argument* axis) = 0;

  };

}

#endif
