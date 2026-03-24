/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolarCalibratorInfo.h"
#include "MEAL/Rotation.h"
#include "Pauli.h"

using namespace std;
using namespace Pulsar;

PolarCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

string PolarCalibrator::Info::get_title () const
{
  return "Polar Decomposition of Jones Matrix";
}

//! Return the number of parameter classes
unsigned PolarCalibrator::Info::get_nclass () const
{
  // gain, 3-D boost, and 3-D rotation
  return 3; 
}

//! Return the name of the specified class
string PolarCalibrator::Info::get_label (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "G (c\\d0\\u)";
  case 1:
    return "sinh\\(2128)\\.m\\b\\u \\(0832)";
  case 2:
    return "sin\\gf\\.n\\b\\u\\(0832)";
  default:
    return "";
  }
}
  
//! Return the number of parameters in the specified class
unsigned PolarCalibrator::Info::get_nparam (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return 1;
  case 1:
  case 2:
    return 3;
  default:
    return 0;
  }
}

template<typename T, typename U>
const Estimate<T,U> sinc (const Estimate<T,U>& u)
{
  T val = 1.0;
  T dval_du = 0.0;

  if (u.val != 0.0)
  {
    val = ::sin(u.val) / u.val; 
    dval_du = (::cos(u.val) - val) / u.val;
  }

  return Estimate<T,U> (val, dval_du*dval_du*u.var);
}

//! Return the estimate of the specified parameter
Estimate<float> 
PolarCalibrator::Info::get_param (unsigned ichan, unsigned iclass,
					  unsigned iparam) const
{
  if( ! calibrator->get_transformation_valid (ichan) )
    return 0;

  if (iclass < 2)
    return PolnCalibrator::Info::get_param (ichan, iclass, iparam);

  const MEAL::Polar* polar
    = dynamic_cast<const MEAL::Polar*> 
    ( calibrator->get_transformation (ichan) );

  if (!polar)
    throw Error (InvalidState, "PolarCalibrator::Info::get_param",
		 "transformation[%d] is not an Polar", ichan);

  const MEAL::Rotation* rotation = polar->get_rotation_transformation();

  vector< Jones<double> > grad;
  Jones<double> R = rotation->evaluate (&grad);

  Quaternion<double, Unitary> q = real( unitary(R) );
  double val = q[iparam + 1];
  double var = 0.0;
  for (unsigned i=0; i<3; i++)
  {
    Quaternion<double, Unitary> dq_dri = real( unitary(grad[i]) );
    double dval_dri = dq_dri[iparam + 1];
    var += dval_dri * dval_dri * rotation->get_variance(i);
  }

  return Estimate<float> (val, var);
}

Calibrator::Info* PolarCalibrator::get_Info () const
{
  return new PolarCalibrator::Info (this);
}

