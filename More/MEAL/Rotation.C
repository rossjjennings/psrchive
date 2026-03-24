/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Rotation.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"

using namespace std;

MEAL::Rotation::Rotation ()
{
  init();
}

void MEAL::Rotation::init()
{
  Parameters* params = new Parameters (this, 3);

  for (unsigned i=0; i<3; i++)
  {
    string index = tostring (i + 1);

    string name = "r_" + index;

    if (very_verbose)
      cerr << "MEAL::Rotation::init name[" << i << "]=" << name << endl;

    params->set_name (i, name);

    string description = "rotation axis_" + index + " times angle (radians)";

    if (very_verbose)
      cerr << "MEAL::Rotation::init description[" << i << "]=" 
           << description << endl;

    params->set_description (i, description);
  }
}

MEAL::Rotation::Rotation (const Vector<3,double>& axis, double angle)
{
  init();
  set_axis_angle (axis,angle);
}

//! Return the name of the class
string MEAL::Rotation::get_name () const
{
  return "Rotation";
}

Vector<3,double> MEAL::Rotation::get_vector () const
{
  Vector<3, double> vect;
  for (unsigned i=0; i<3; i++)
    vect[i] = get_param(i);
  return vect;
}

//! Get the unit-vector along which the rotation occurs
Vector<3,double> MEAL::Rotation::get_axis () const
{
  Vector<3, double> vect = get_vector();
  double mod = sqrt (vect * vect);
  if (mod != 0.0)
    vect /= mod;

  return vect;
}

//! Set the unit-vector along which the rotation occurs and the angle in radians
void MEAL::Rotation::set_axis_angle (Vector<3,double> axis, double angle) 
{
  // ensure that axis is a unit vector
  double mod = sqrt (axis * axis);
  if (mod != 0.0)
    axis /= mod;

  set_vector (angle * axis);
}

void MEAL::Rotation::set_vector (const Vector<3,double>& vect)
{
  for (unsigned i=0; i<3; i++)
    set_param(i, vect[i]);
}

double MEAL::Rotation::get_phi () const
{
  Vector<3, double> vect = get_vector();
  return sqrt (vect * vect);
}

//! Calculate the Jones matrix and its gradient
void MEAL::Rotation::calculate (Jones<double>& result,
				vector<Jones<double> >* grad)
{
  Vector<3, double> vect = get_vector();

  if (verbose)
    cerr << "MEAL::Rotation::calculate vect=" << vect << endl;

  // calculate the Rotation component
  double phi = sqrt (vect * vect);

  double cos_phi = cos(phi);
  double sinc_phi = 1.0;
  double dsinc_phi = 0.0;

  if (phi != 0) {
    sinc_phi = sin(phi)/phi;
    dsinc_phi = (cos_phi - sinc_phi)/phi;
  }

  // the Rotation quaternion
  Quaternion<double, Unitary> rotation (cos_phi, sinc_phi * vect);
  result = convert (rotation);

  if (!grad)
    return;

  // build the partial derivatives with respect to rotation vect-vector
  for (unsigned i=0; i<3; i++) {
    
    // partial derivative of Rotation[0]=cos(phi) wrt Rotation[i+1]=vect[i]
    double dcos_phi_dvecti = - vect[i] * sinc_phi;
    
    double dphi_dvecti = 1.0;
    if (phi != 0)
      dphi_dvecti = vect[i] / phi;

    Vector<3, double> dvect = dsinc_phi * dphi_dvecti * vect;
    dvect[i] += sinc_phi;

    Quaternion<double, Unitary> drotation_dvecti (dcos_phi_dvecti, dvect);
    
    // set the partial derivative wrt this parameter
    (*grad)[i] = convert (drotation_dvecti);
    
  }
  
  if (verbose)
  {
    cerr << "MEAL::Rotation::calculate gradient" << endl;
    for (unsigned i=0; i<3; i++)
      cerr << "   " << (*grad)[i] << endl;
  }
}

