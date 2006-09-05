/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Boost1.h"
#include "MEAL/OneParameter.h"
#include "Pauli.h"

using namespace std;

MEAL::Boost1::Boost1 (const Vector<3,double>& _axis) 
{
  OneParameter* param = new OneParameter (this);
  param->set_param_name ("boost");
  set_axis (_axis);
}

//! Return the name of the class
string MEAL::Boost1::get_name () const
{
  return "Boost1";
}

void MEAL::Boost1::set_axis (const Vector<3,double>& _axis)
{
  double norm = _axis * _axis;
  axis = _axis / sqrt(norm);
}

//! Get the unit-vector along which the boost occurs
Vector<3, double> MEAL::Boost1::get_axis () const
{
  return axis;
}

void MEAL::Boost1::set_beta (const Estimate<double>& beta)
{
  set_Estimate (0, beta);
}

Estimate<double> MEAL::Boost1::get_beta () const
{
  return get_Estimate (0);
}

//! Return the Jones matrix and its gradient
void MEAL::Boost1::calculate (Jones<double>& result,
			      vector<Jones<double> >* grad)
{
  double beta = get_param(0);

  if (verbose)
    cerr << "MEAL::Boost1::calculate axis=" << axis 
	 << " beta=" << beta << endl;

  double sinh_beta = sinh (beta);
  double cosh_beta = cosh (beta);

  Quaternion<double, Hermitian> boost (cosh_beta, sinh_beta*axis);
  result = convert (boost);

  if (!grad)
    return;

  Quaternion<double, Hermitian> dboost_dbeta (sinh_beta, cosh_beta*axis);
  (*grad)[0] = convert (dboost_dbeta);

  if (verbose) {
    cerr << "MEAL::Boost1::calculate gradient" << endl;
    cerr << "   " << (*grad)[0] << endl;
  }
  
}
