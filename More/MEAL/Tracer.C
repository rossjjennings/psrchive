#include "Calibration/Tracer.h"
#include "Calibration/Model.h"

Calibration::Tracer::Tracer (Model* _model, unsigned param)
{
  if (_model)
    watch (_model, param);
}

Calibration::Tracer::~Tracer ()
{
  if (Model::verbose)
    cerr << "Calibration::Tracer::~Tracer" << endl;

  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);
}

void Calibration::Tracer::watch (Model* _model, unsigned param) try {

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
  throw error += "Calibration::Tracer::watch";
}

//! Method called when a Model attribute has changed
void Calibration::Tracer::attribute_changed (Model::Attribute attribute) try {

  if (!model)
    throw Error (InvalidState, "Calibration::Tracer::attribute_changed",
		 "method called with no model being watched");

  double latest = model->get_param (parameter);

  if (latest != current_value)
    report ();

  current_value = latest;
}
catch (Error& error) {
  throw error += "Calibration::Tracer::attribute_changed";
}

void Calibration::Tracer::report () try  {

  cerr << "Calibration::Tracer " << model->get_name() << " "
       << model->get_param_name(parameter) << " " 
       << model->get_param(parameter) << endl;

}
catch (Error& error) {
  throw error += "Calibration::Tracer::report";
}

