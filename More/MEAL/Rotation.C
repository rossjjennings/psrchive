#include "Rotation.h"
#include "Pauli.h"

Calibration::Rotation::Rotation () : OptimizedComplex2 (1)
{
}

Calibration::Rotation::Rotation (const Vector<double, 3>& _axis) 
  : OptimizedComplex2 (1)
{
  axis = _axis;
}

//! Return the name of the class
string Calibration::Rotation::get_name () const
{
  return "Rotation";
}

//! Return the name of the specified parameter
string Calibration::Rotation::get_param_name (unsigned index) const
{
  if (index == 0)
    return "rotation";
  else
    return "ERROR";
}

void Calibration::Rotation::set_phi (double radians)
{
  if (verbose)
    cerr << "Calibration::Rotation::set_phi " << radians << endl;

  set_param (0, radians);
}

double Calibration::Rotation::get_phi () const
{
  return get_param (0);
}

//! Calculate the Jones matrix and its gradient
void Calibration::Rotation::calculate (Jones<double>& result,
				       vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  if (verbose)
    cerr << "Calibration::Rotation::calculate axis=" << axis 
	 << " phi=" << phi << endl;

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  if (grad) {
    Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);

    (*grad)[0] = convert (drotation_dphi);

    if (verbose)
      cerr << "Calibration::Rotation::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);
}

void Calibration::Rotation::set_axis (const Vector<double, 3>& _axis)
{
  axis = _axis;
}

