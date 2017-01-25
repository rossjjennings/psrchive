//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/EvaluationTracer.h

#ifndef __MEAL_EvaluationTracer_H
#define __MEAL_EvaluationTracer_H

#include "MEAL/Tracer.h"

namespace MEAL
{

  //! Traces changes in the result of a function
  template<class T>
  class EvaluationTracer : public Tracer
  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    EvaluationTracer (T* _model = 0) { if (_model) watch (_model); }

    //! Trace the specified model
    virtual void watch (T* _model) { Tmodel=_model; Tracer::watch(_model); }

  protected:

    //! The model to watch
    Reference::To<T> Tmodel;

    //! The current value of the result
    Result current_value;

    //! Method called to report parameter value
    void report ()
    {
      Result val=Tmodel->evaluate();
      if (val == current_value)
        return;
      current_value = val;
      std::cerr << "EvaluationTracer::report " << model->get_name() << " "
                << current_value << std::endl;
    }

  };

}

#endif

