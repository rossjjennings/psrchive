//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ArgumentPolicyAdapter.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 15:30:56 $
   $Author: straten $ */

#ifndef __MEAL_ArgumentPolicyAdapter_H
#define __MEAL_ArgumentPolicyAdapter_H

#include "MEAL/ArgumentPolicy.h"

namespace MEAL {

  //! Adapts an object with the ArgumentPolicy interface
  template<class T>
  class ArgumentPolicyAdapter : public ArgumentPolicy {

  public:

    //! Default constructor
    ArgumentPolicyAdapter (Function* context, T* adapt)
     : ArgumentPolicy (context), adaptee (adapt) { }

    //! Clone operator 
    ArgumentPolicyAdapter* clone (Function* context) const
    { return new ArgumentPolicyAdapter (context, adaptee); }

    //! Set the independent variable of the specified dimension
    void set_argument (unsigned dimension, Argument* axis)
    { adaptee->set_argument (dimension, axis); }

  protected:

    //! The adapted object
    Reference::To<T> adaptee;

  };

}

#endif
