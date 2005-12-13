#include "Calibration/Faraday.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"

using namespace std;

Calibration::Faraday::Faraday () 
{
  MEAL::OneParameter* parm = new MEAL::OneParameter (this);
  parm->set_param_name (0, "RM");

  rotation.set_axis (Pauli::basis.get_basis_vector(2));

  reference_frequency = 0.0;
  frequency = 0.0;
}

//! Return the name of the class
string Calibration::Faraday::get_name () const
{
  return "Faraday";
}

//! Set the rotation measure
void Calibration::Faraday::set_rotation_measure (const Estimate<double>& rm)
{
  set_Estimate (0, rm);
}

//! Get the rotation measure
Estimate<double> Calibration::Faraday::get_rotation_measure () const
{
  return get_Estimate (0);
}


//! Set the reference frequency in MHz
void Calibration::Faraday::set_reference_frequency (double MHz)
{
  if (reference_frequency == MHz)
    return;

  reference_frequency = MHz;
  set_evaluation_changed ();
}

//! Get the reference frequency in MHz
double Calibration::Faraday::get_reference_frequency () const
{
  return reference_frequency;
}


//! Set the frequency in MHz
void Calibration::Faraday::set_frequency (double MHz)
{
  if (frequency == MHz)
    return;

  frequency = MHz;
  set_evaluation_changed ();
}

//! Get the frequency in MHz
double Calibration::Faraday::get_frequency () const
{
  return frequency;
}

void Calibration::Faraday::set_axis (const Vector<3, double>& axis)
{
  rotation.set_axis (axis);
  copy_evaluation_changed (rotation);
}

double Calibration::Faraday::get_rotation () const
{
  // speed of light in m/s
  double speed_of_light = 299792458;

  double lambda_0 = 0;
  if (reference_frequency != 0.0)
    lambda_0 = speed_of_light / (reference_frequency * 1e6);

  double lambda = 0;
  if (frequency != 0.0)
    lambda = speed_of_light / (frequency * 1e6);

  // The OneParameter policy stores the rotation measure
  double rotation_measure = get_param (0);

  // Longer wavelength -> greater rotation
  return rotation_measure * (lambda*lambda - lambda_0*lambda_0);
}

//! Calculate the Jones matrix and its gradient
void Calibration::Faraday::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  /* remember that the Rotation transformation rotates the basis, so the
     effect on the coherency matrix is negative */

  rotation.set_phi ( -get_rotation() );
  result = rotation.evaluate (grad);

  if (!grad)
    return;

  if (grad->size() != 1)
    throw Error (InvalidState, "Calibration::Faraday::calculate",
                 "gradient.size=%d != 1", grad->size());

  double dphi_dRM = rotation.get_phi() / get_param(0);

  (*grad)[0] *= dphi_dRM;
}

