//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/FunctionPolicy.h,v $
   $Revision: 1.4 $
   $Date: 2007/10/02 05:19:48 $
   $Author: straten $ */

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

  protected:

    //! Return the context to derived classes
    Function* get_context () const { return context; }

  private:

    friend class Function;

    //! The context
    Function* context;

  };

}

#endif
