#include "MEPL/Tracer.h"
#include "MEPL/Function.h"

Model::Tracer::Tracer (Function* _model, unsigned param)
{
  if (_model)
    watch (_model, param);
}

Model::Tracer::~Tracer ()
{
  if (Function::verbose)
    cerr << "Model::Tracer::~Tracer" << endl;

  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);
}

void Model::Tracer::watch (Function* _model, unsigned param) try {

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
  throw error += "Model::Tracer::watch";
}

//! Method called when a Model attribute has changed
void Model::Tracer::attribute_changed (Function::Attribute attribute) try {

  if (!model)
    throw Error (InvalidState, "Model::Tracer::attribute_changed",
		 "method called with no model being watched");

  double latest = model->get_param (parameter);

  if (latest != current_value)
    report ();

  current_value = latest;
}
catch (Error& error) {
  throw error += "Model::Tracer::attribute_changed";
}

void Model::Tracer::report () try  {

  cerr << "Model::Tracer " << model->get_name() << " "
       << model->get_param_name(parameter) << " " 
       << model->get_param(parameter) << endl;

}
catch (Error& error) {
  throw error += "Model::Tracer::report";
}

