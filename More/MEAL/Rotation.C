#include "MEPL/Rotation.h"
#include "Pauli.h"

Model::Rotation::Rotation () : OptimizedComplex2 (1)
{
}

Model::Rotation::Rotation (const Vector<double, 3>& _axis) 
  : OptimizedComplex2 (1)
{
  axis = _axis;
}

//! Return the name of the class
string Model::Rotation::get_name () const
{
  return "Rotation";
}

//! Return the name of the specified parameter
string Model::Rotation::get_param_name (unsigned index) const
{
  if (index == 0)
    return "rotation";
  else
    return "ERROR";
}

void Model::Rotation::set_phi (double radians)
{
  if (verbose)
    cerr << "Model::Rotation::set_phi " << radians << endl;

  set_param (0, radians);
}

double Model::Rotation::get_phi () const
{
  return get_param (0);
}

//! Calculate the Jones matrix and its gradient
void Model::Rotation::calculate (Jones<double>& result,
				       vector<Jones<double> >* grad)
{
  double phi = get_param(0);

  if (verbose)
    cerr << "Model::Rotation::calculate axis=" << axis 
	 << " phi=" << phi << endl;

  double sin_phi = sin (phi);
  double cos_phi = cos (phi);

  if (grad) {
    Quaternion<double, Unitary> drotation_dphi (-sin_phi, cos_phi*axis);

    (*grad)[0] = convert (drotation_dphi);

    if (verbose)
      cerr << "Model::Rotation::calculate gradient" << endl
	   << "   " << (*grad)[0] << endl;
  }

  Quaternion<double, Unitary> rotation (cos_phi, sin_phi*axis);
  result = convert (rotation);
}

void Model::Rotation::set_axis (const Vector<double, 3>& _axis)
{
  axis = _axis;
}

