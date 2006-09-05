/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Rotation1.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"

using namespace std;

MEAL::Rotation1::Rotation1 (const Vector<3,double>& _axis) 
{
  OneParameter* param = new OneParameter (this);
  param->set_param_name ("rotation");
  axis = _axis;
}

//! Return the name of the class
std::string MEAL::Rotation1::get_name () const
{
  return "Rotation1";
}

void MEAL::Rotation1::set_axis (const Vector<3,double>& _axis)
{
  axis = _axis;
}

Vector<3,double> MEAL::Rotation1::get_axis () const
{
  return axis;
}


void MEAL::Rotation1::set_phi (const Estimate<double>& radians)
{
  if (verbose)
    cerr << "MEAL::Rotation1::set_phi " << radians << endl;

  set_Estimate (0, radians);
}

Estimate<double> MEAL::Rotation1::get_phi () const
{
  return get_Estimate (0);
}

void MEAL::Rotation1::set_parameter_policy (OneParameter* policy)
{
  OneParameter* current = dynamic_cast<OneParameter*>(parameter_policy.get());
  *policy = *current;
  parameter_policy = policy;
}

//! Calculate the Jones matrix and its gradient
void MEAL::Rotation1::calculate (Jones<double>& result,
				 std::vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate axis=" << axis 
	 << " phi=" << phi << endl;

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);

  if (!grad)
    return;

  Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);
  (*grad)[0] = convert (drotation_dphi);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate gradient" << endl
	 << "   " << (*grad)[0] << endl;
}
