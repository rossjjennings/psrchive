//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/FunctionPolicy.h

#ifndef __MEAL_FunctionPolicy_H
#define __MEAL_FunctionPolicy_H

#include "ReferenceAble.h"

namespace MEAL {

  class Function;

  //! Policies that customize Function behaviours
  class FunctionPolicy : public Reference::Able {

  public:

    //! Default constructor
    FunctionPolicy (Function* context=0);

    //! Destructor
    ~FunctionPolicy ();
    
    //! Return the Function for whom this policy operates
    Function* get_context () const { return context; }

  private:

    friend class Function;

    //! The context
    Function* context;

  };

  template<class T>
  class FunctionPolicyTraits
  {
  public:
    static void composite_component (T* composite, T* component)
    {
      // do nothing
    }
  };
  
}

#endif
