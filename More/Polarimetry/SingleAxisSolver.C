#include "Calibration/SingleAxisSolver.h"
#include "Calibration/SingleAxis.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarValue.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;

Calibration::SingleAxisSolver::SingleAxisSolver ()
{
  init ();
}

Calibration::SingleAxisSolver::SingleAxisSolver (const SingleAxisSolver&)
{
  init ();
}

Calibration::SingleAxisSolver::~SingleAxisSolver ()
{

}

Calibration::SingleAxisSolver&
Calibration::SingleAxisSolver::operator = (const SingleAxisSolver&)
{
  return *this;
}

void
Calibration::SingleAxisSolver::set_input (const Stokes< Estimate<double> >& S)
{
  if (MEAL::Function::verbose)
    cerr << "Calibration::SingleAxis::set_input=" << S << endl;

  for (unsigned ipol=0; ipol<S.size(); ipol++)
    input[ipol].get_expression()->set_Estimate( 0, S[ipol] );
}

void
Calibration::SingleAxisSolver::set_output (const Stokes< Estimate<double> >& S)
{
  if (MEAL::Function::verbose)
    cerr << "Calibration::SingleAxis::set_output=" << S << endl;

  for (unsigned ipol=0; ipol<S.size(); ipol++)
    output[ipol].get_expression()->set_Estimate( 0, S[ipol] );
}

//! Set the SingleAxis parameters with the current solution
void Calibration::SingleAxisSolver::solve (SingleAxis* model)
{
  Vector<double, 3> model_axis = model->get_axis();

  for (unsigned i=0; i<3; i++)
    axis[i].get_expression()->set_param( 0, model_axis[i] );

  model->set_diff_phase( diff_phase.get_Estimate() );

  model->set_diff_gain( diff_gain.get_Estimate() );

  model->set_gain( gain.get_Estimate() );
}

//! Solve for gain, boost, and rotation given input and output states
void Calibration::SingleAxisSolver::init ()
{
  for (unsigned ipol=0; ipol<input.size(); ipol++) {
    input[ipol]  = *(new MEAL::ScalarValue);
    output[ipol] = *(new MEAL::ScalarValue);
  }

  for (unsigned i=0; i<3; i++)
    axis[i] = *(new MEAL::ScalarValue);

  // decompose the input unit vector

  Vector<MEAL::ScalarMath, 3> input_vector = input.get_vector();
  MEAL::ScalarMath input_magnitude = sqrt(input_vector * input_vector);

  Vector<MEAL::ScalarMath, 3> input_unit = input_vector/input_magnitude;

  Vector<MEAL::ScalarMath, 3> input_parallel = (input_unit * axis) * axis;
  Vector<MEAL::ScalarMath, 3> input_perp = input_unit - input_parallel;

  // decompose the output unit vector

  Vector<MEAL::ScalarMath, 3> output_vector = output.get_vector();
  MEAL::ScalarMath output_magnitude = sqrt(output_vector * output_vector);

  Vector<MEAL::ScalarMath, 3> output_unit = output_vector/output_magnitude;

  Vector<MEAL::ScalarMath, 3> output_parallel = (output_unit * axis) * axis;
  Vector<MEAL::ScalarMath, 3> output_perp = output_unit - output_parallel;

  // find the rotation
  MEAL::ScalarMath cos_2phi = input_perp * output_perp;
  MEAL::ScalarMath sin_2phi = cross( output_perp, input_perp ) * axis;

  diff_phase = 0.5 * atan2 (sin_2phi, cos_2phi);

  // find the boost
  MEAL::ScalarMath input_p = input_vector * axis;
  MEAL::ScalarMath output_p = output_vector * axis;

  MEAL::ScalarMath tanh_2beta 
    = ( input_p * output[0] - output_p * input[0] ) /
      ( input_p * output_p - input[0] * output[0] );

  diff_gain = 0.5 * atanh (tanh_2beta);

  MEAL::ScalarMath Gsquared
    = ( output[0] * output[0] - output_p * output_p ) /
      ( input[0] * input[0] - input_p * input_p );

  MEAL::ScalarMath G = sqrt (Gsquared);

  gain = sqrt(G);

}
