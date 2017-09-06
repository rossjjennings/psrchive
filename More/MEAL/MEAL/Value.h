//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Complex2Value.h

#ifndef __MEAL_Value_H
#define __MEAL_Value_H

#include "MEAL/NoParameters.h"
#include "MEAL/NotCached.h"

namespace MEAL {

  //! Represents a value with no free parameters
  template<typename T>
  class Value : public T {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    Value () { init(); }

    //! Construct with initial value
    Value (const Result& _value) { init(); value = _value; }

    //! Set the value
    void set_value (const Result& _value)
    {
      if (value == _value)
	return;

      value = _value;
      this->set_evaluation_changed();
    }

    //! Get the value
    const Result& get_value () const { return value; }

    //! Return the name of the class
    std::string get_name () const
    { return "Value<" + std::string(T::Name)+ ">"; }

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (Result& result, std::vector<Result>* grad=0)
    {
      result = value;
      if (grad) grad->resize(0);
    }

    void init ()
    {
      this->parameter_policy = new NoParameters;
      this->evaluation_policy = new NotCached<T> (this);
    }

    //! The value
    Result value;

  };

}

#endif
