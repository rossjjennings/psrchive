#include "Calibration/MeasurementEquation.h"

using namespace std;

Calibration::MeasurementEquation::MeasurementEquation ()
{
  current_input = 0;
  current_xform = 0;
}

Calibration::MeasurementEquation::~MeasurementEquation ()
{
}

/*! This method unmaps the old input before mapping the new */
void Calibration::MeasurementEquation::set_input (Complex2* _input)
{
  if (!_input)
    return;

  if (inputs[current_input]) {
    if (very_verbose)
      cerr << "Calibration::MeasurementEquation::set_input"
	" unmap old input" << endl;

    unmap (inputs[current_input], false);
  }

  inputs[current_input] = _input;
  input = inputs[current_input];

  if (very_verbose)
    cerr << "Calibration::MeasurementEquation::set_input"
      " map new input" << endl;

  map (inputs[current_input]);
  // inputs[current_input]->name = "MeasurementEquation::input";
}

/*!
  \post current_input will be set to the newly added input
*/
void Calibration::MeasurementEquation::add_input (Complex2* _input)
{
  if (very_verbose) cerr << "Calibration::MeasurementEquation::add_input"
		      " input=" << _input << endl;

  current_input = inputs.size();
  inputs.resize (current_input + 1);

  if (_input)
    set_input (_input);

  set_input_index (current_input);
}


unsigned Calibration::MeasurementEquation::get_num_input () const
{
  return inputs.size();
}

unsigned Calibration::MeasurementEquation::get_input_index () const
{
  return current_input;
}

void Calibration::MeasurementEquation::set_input_index (unsigned index)
{
  if (index >= inputs.size())
    throw Error (InvalidRange,
		 "Calibration::MeasurementEquation::set_input_index",
		 "index=%d >= ninput=%d", index, inputs.size());

  if (verbose) cerr << "Calibration::MeasurementEquation::set_input_index "
		    << index << endl;

  if (current_input != index)
    set_evaluation_changed ();

  current_input = index;
  input = inputs[current_input];

}


/*! This method unmaps the old transformation before mapping the new */
void 
Calibration::MeasurementEquation::set_transformation (Complex2* _xform)
{
  if (!_xform)
    return;

  if (xforms[current_xform]) {
    if (very_verbose)
      cerr << "Calibration::MeasurementEquation::set_transformation"
	" unmap old transformation" << endl;

    unmap (xforms[current_xform], false);
  }

  xforms[current_xform] = _xform;
  transformation = xforms[current_xform];

  if (very_verbose)
    cerr << "Calibration::MeasurementEquation::set_transformation"
      " map new transformation" << endl;

  map (xforms[current_xform]);
  // xforms[current_xform]->name = "MeasurementEquation::xform";
}


/*!
  \post current_xform will be set to the newly added transformation
*/
void Calibration::MeasurementEquation::add_transformation (Complex2* _xform)
{
  if (very_verbose) 
    cerr << "Calibration::MeasurementEquation::add_transformation"
      " xform=" << _xform << endl;

  current_xform = xforms.size();
  xforms.resize (current_xform + 1);

  if (_xform)
    set_transformation (_xform);

  set_transformation_index (current_xform);
}
 
unsigned Calibration::MeasurementEquation::get_num_transformation () const
{
  return xforms.size();
}

unsigned Calibration::MeasurementEquation::get_transformation_index () const
{
  return current_xform;
}

void
Calibration::MeasurementEquation::set_transformation_index (unsigned index)
{
  if (index >= xforms.size())
    throw Error (InvalidRange, 
		 "Calibration::MeasurementEquation::set_transformation_index",
		 "index=%d >= npath=%d", index, xforms.size());

  if (verbose)
    cerr << "Calibration::MeasurementEquation::set_transformation_index "
	 << index << endl;

  if (current_xform != index)
    set_evaluation_changed ();

  current_xform = index;
  transformation = xforms[current_xform];
}
