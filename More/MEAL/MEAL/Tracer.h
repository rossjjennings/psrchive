//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Tracer.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_Tracer_H
#define __Model_Tracer_H

#include "MEPL/Function.h"

namespace Model {

  //! This class provides verbose output of changes to a single parameter
  class Tracer : public Reference::Able {

  public:

    //! Default constructor
    Tracer (Function* model = 0, unsigned param = 0);

    //! Destructor
    ~Tracer ();

    //! Add the Function parameters to the running mean
    virtual void watch (Function* model, unsigned param);

  protected:

    //! The model to watch
    Reference::To<Function> model;

    //! The parameter to watch
    unsigned parameter;

    //! The current value of the watched parameter
    double current_value;

    //! Method called when a Function attribute has changed
    void attribute_changed (Function::Attribute attribute);

    //! Method called to report parameter value
    void report ();

  };

}

#endif

