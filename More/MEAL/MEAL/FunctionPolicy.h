//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/FunctionPolicy.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:35:25 $
   $Author: straten $ */

#ifndef __MEAL_FunctionPolicy_H
#define __MEAL_FunctionPolicy_H

#include "ReferenceAble.h"

namespace MEAL {

  class Function;

  //! Abstract base class of Function policies
  class FunctionPolicy : public Reference::Able {

  public:

    //! Default constructor
    FunctionPolicy (Function* _context = 0) { context = _context; }

    //! Virtual destructor
    virtual ~FunctionPolicy () { }

  protected:

    //! Return the context to derived classes
    Function* get_context () const { return context; }

  private:

    //! The context
    Function* context;

  };

}

#endif
