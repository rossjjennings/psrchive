#include "MEAL/Rotation.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"

using namespace std;

void MEAL::Rotation::init ()
{
  OneParameter* param = new OneParameter (this);
  param->set_param_name (0, "rotation");
}

MEAL::Rotation::Rotation ()
{
  init ();
  axis = Vector<double, 3> (1,0,0);
}

MEAL::Rotation::Rotation (const Vector<double, 3>& _axis) 
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

void MEAL::Rotation::set_axis (const Vector<double, 3>& _axis)
{
  axis = _axis;
}

