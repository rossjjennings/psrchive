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
  set_axis( _axis );
}

//! Return the name of the class
std::string MEAL::Rotation1::get_name () const
{
  return "Rotation1";
}

void MEAL::Rotation1::set_axis (const Vector<3,double>& _axis)
{
  if (normsq(_axis) == 0.0)
    throw Error (InvalidState, "MEAL::Rotation1::set_axis",
		 "invalid rotation axis = 0");

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
  if (policy) {
    OneParameter* current = dynamic_kast<OneParameter>(parameter_policy);
    if (current)
      *policy = *current;
  }
  Function::set_parameter_policy( policy );
}

//! Calculate the Jones matrix and its gradient
void MEAL::Rotation1::calculate (Jones<double>& result,
				 std::vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate axis=" << axis 
	 << " phi=" << phi << " det(J)=" << det(result) << endl;

  if (!grad)
    return;

  Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);
  (*grad)[0] = convert (drotation_dphi);

  if (verbose)
    cerr << "MEAL::Rotation1::calculate gradient" << endl
	 << "   " << (*grad)[0] << endl;
}
