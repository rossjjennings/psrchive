//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Tracer.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_Tracer_H
#define __Calibration_Tracer_H

#include "Calibration/Model.h"

namespace Calibration {

  //! This class provides verbose output of changes to a single parameter
  class Tracer : public Reference::Able {

  public:

    //! Default constructor
    Tracer (Model* model = 0, unsigned param = 0);

    //! Destructor
    ~Tracer ();

    //! Add the Model parameters to the running mean
    virtual void watch (Model* model, unsigned param);

  protected:

    //! The model to watch
    Reference::To<Model> model;

    //! The parameter to watch
    unsigned parameter;

    //! The current value of the watched parameter
    double current_value;

    //! Method called when a Model attribute has changed
    void attribute_changed (Model::Attribute attribute);

    //! Method called to report parameter value
    void report ();

  };

}

#endif

