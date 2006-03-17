/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Rotation.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"

using namespace std;

void MEAL::Rotation::init ()
{
  OneParameter* param = new OneParameter (this);
  param->set_param_name ("rotation");
}

MEAL::Rotation::Rotation ()
{
  init ();
  axis = Vector<3,double> (1,0,0);
}

MEAL::Rotation::Rotation (const Vector<3,double>& _axis) 
{
  init ();
  axis = _axis;
}

//! Return the name of the class
std::string MEAL::Rotation::get_name () const
{
  return "Rotation";
}


void MEAL::Rotation::set_phi (double radians)
{
  if (verbose)
    cerr << "MEAL::Rotation::set_phi " << radians << endl;

  set_param (0, radians);
}

double MEAL::Rotation::get_phi () const
{
  return get_param (0);
}

void MEAL::Rotation::set_parameter_policy (OneParameter* policy)
{
  OneParameter* current = dynamic_cast<OneParameter*>(parameter_policy.get());
  *policy = *current;
  parameter_policy = policy;
}

//! Calculate the Jones matrix and its gradient
void MEAL::Rotation::calculate (Jones<double>& result,
				std::vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  if (verbose)
    cerr << "MEAL::Rotation::calculate axis=" << axis 
	 << " phi=" << phi << endl;

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  if (grad) {
    Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);

    (*grad)[0] = convert (drotation_dphi);

    if (verbose)
      cerr << "MEAL::Rotation::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);
}

void MEAL::Rotation::set_axis (const Vector<3,double>& _axis)
{
  axis = _axis;
}

