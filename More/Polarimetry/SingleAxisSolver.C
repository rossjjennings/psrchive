/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"

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

static Stokes< Estimate<double> > zero;

void
Calibration::SingleAxisSolver::set_input (const Stokes< Estimate<double> >& S)
{
  if (MEAL::Function::verbose)
    cerr << "Calibration::SingleAxis::set_input=" << S << endl;

  if (S == zero)
    throw Error (InvalidParam, "Calibration::SingleAxisSolver::set_input",
                 "invalid Stokes parameters equal zero");

  // Convert to the natural basis
  Quaternion<Estimate<double>,Hermitian> q = natural (S);

  for (unsigned ipol=0; ipol<S.size(); ipol++)
    input[ipol].get_expression()->set_Estimate( 0, q[ipol] );
}

void
Calibration::SingleAxisSolver::set_output (const Stokes< Estimate<double> >& S)
{
  if (MEAL::Function::verbose)
    cerr << "Calibration::SingleAxis::set_output=" << S << endl;

  if (S == zero)
    throw Error (InvalidParam, "Calibration::SingleAxisSolver::set_output",
	                       "invalid Stokes parameters equal zero");

  Quaternion<Estimate<double>,Hermitian> q = natural (S);

  for (unsigned ipol=0; ipol<S.size(); ipol++)
    output[ipol].get_expression()->set_Estimate( 0, q[ipol] );
}

//! Set the SingleAxis parameters with the current solution
void Calibration::SingleAxisSolver::solve (SingleAxis* model) try
{
  Vector<3, double> model_axis = model->get_axis();

  for (unsigned i=0; i<3; i++)
    axis[i].get_expression()->set_param( 0, model_axis[i] );

  model->set_diff_phase( diff_phase.get_Estimate() );
  model->set_diff_gain( diff_gain.get_Estimate() );
  model->set_gain( gain.get_Estimate() );

}
catch (Error& error)
{
  error << "\n\t"
    "input=" << input
	<< "\n\t"
    "output=" << output;

  throw error += "Calibration::SingleAxisSolver::solve";
}

//! Solve for gain, boost, and rotation given input and output states
void Calibration::SingleAxisSolver::init ()
{
  for (unsigned ipol=0; ipol<input.size(); ipol++) {
    input[ipol]  = *(new MEAL::ScalarParameter);
    output[ipol] = *(new MEAL::ScalarParameter);
  }

  for (unsigned i=0; i<3; i++)
    axis[i] = *(new MEAL::ScalarParameter);

  // decompose the input unit vector

  Vector<3, MEAL::ScalarMath> input_vector = input.get_vector();
  MEAL::ScalarMath input_p  = input_vector * axis;

  Vector<3, MEAL::ScalarMath> input_parallel = input_p * axis;
  Vector<3, MEAL::ScalarMath> input_perp = input_vector - input_parallel;

  // decompose the output unit vector

  Vector<3, MEAL::ScalarMath> output_vector = output.get_vector();
  MEAL::ScalarMath output_p = output_vector * axis;

  Vector<3, MEAL::ScalarMath> output_parallel = output_p * axis;
  Vector<3, MEAL::ScalarMath> output_perp = output_vector - output_parallel;

  // find the rotation
  MEAL::ScalarMath A_cos_2phi = input_perp * output_perp;
  MEAL::ScalarMath A_sin_2phi = cross( input_perp, output_perp ) * axis;

  diff_phase = -0.5 * atan2 (A_sin_2phi, A_cos_2phi);

  // find the boost
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
