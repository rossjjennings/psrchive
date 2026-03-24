//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Variable.h

#ifndef __MEAL_Variable_H
#define __MEAL_Variable_H

#include "MEAL/Argument.h"

namespace MEAL {

  //! Pure virtual base class of a transformation with an argument
  /*! This pure virtual template base class defines the interface by
    which a model and its argument can be managed together. */

  template<class T>
  class Variable : public Reference::Able
  {
  public:

    //! The transformation
    virtual T* get_transformation () = 0;

    //! Its argument
    virtual MEAL::Argument* get_argument () = 0;    
  };


}

#endif

