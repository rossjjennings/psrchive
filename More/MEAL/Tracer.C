/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Tracer.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::Tracer::Tracer (Function* _model, unsigned param)
{
  if (_model)
    watch (_model, param);
}

MEAL::Tracer::~Tracer ()
{
  if (Function::verbose)
    cerr << "MEAL::Tracer::~Tracer" << endl;

  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);
}

void MEAL::Tracer::watch (Function* _model, unsigned param) try {

  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);

  model = _model;
  parameter = param;

  if (!model)
    return;

  model->changed.connect (this, &Tracer::attribute_changed);
  report ();

}
catch (Error& error) {
  throw error += "MEAL::Tracer::watch";
}

//! Method called when a Function attribute has changed
void MEAL::Tracer::attribute_changed (Function::Attribute attribute) try {

  if (!model)
    throw Error (InvalidState, "MEAL::Tracer::attribute_changed",
		 "method called with no model being watched");

  double latest = model->get_param (parameter);

  if (latest != current_value)
    report ();

  current_value = latest;
}
catch (Error& error) {
  throw error += "MEAL::Tracer::attribute_changed";
}

void MEAL::Tracer::report () try  {

  cerr << "MEAL::Tracer " << model->get_name() << " "
       << model->get_param_name(parameter) << " " 
       << model->get_param(parameter) << endl;

}
catch (Error& error) {
  throw error += "MEAL::Tracer::report";
}

