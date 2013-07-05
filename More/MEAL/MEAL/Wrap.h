//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Wrap.h

#ifndef __MEAL_Wrap_H
#define __MEAL_Wrap_H

#include "MEAL/Evaluable.h"
#include "Error.h"

namespace MEAL {

  //! Wrap another function
  /*! The evaluation and parameter policies are copied, and the calculate
    method is implemented with a stub that should never be called. */
  template<typename Base>
  class Wrap : public Base 
  {

  public:

    //! The return type of the evaluate method
    typedef typename Base::Result Result;

    //! Set the function to wrap
    void wrap (Evaluable<Result>* _model)
    {
      model = _model;
      this->copy_parameter_policy  (model);
      this->copy_evaluation_policy (model);
    }

  protected:

    //! The calculate method should never be called
    void calculate (Result& result, std::vector<Result>*)
    {
      throw Error (InvalidState, 
		   "Wrap<" + std::string(Base::Name) + ">::calculate",
		   "not implemented - please use wrap method");
    }

    //! The wrapped function
    Reference::To< Evaluable<Result> > model;
  };

}

#endif
