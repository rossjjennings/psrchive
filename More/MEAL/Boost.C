#include "MEPL/Boost.h"
#include "Pauli.h"

Model::Boost::Boost () : OptimizedComplex2 (3)
{
}

Model::Boost::Boost (const Vector<double, 3>& _axis) 
  : OptimizedComplex2 (1)
{
  set_axis (_axis);
}

//! Return the name of the class
string Model::Boost::get_name () const
{
  return "Boost";
}

//! Return the name of the specified parameter
string Model::Boost::get_param_name (unsigned index) const
{
  if (index == 0)
    return "boost";
  else
    return "ERROR";
}

void Model::Boost::set_axis (const Vector<double, 3>& _axis)
{
  double norm = _axis * _axis;
  axis = _axis / sqrt(norm);

  resize (1);
}

//! Get the unit-vector along which the boost occurs
Vector<double, 3> Model::Boost::get_axis (double* beta) const
{
  if (get_nparam() == 1) {
    if (beta)
      *beta = get_param(0);
    return axis;
  }
  else {

    Vector<double, 3> Gibbs;
    for (unsigned i=0; i<3; i++)
      Gibbs[i] = get_param(i);
    double mod = sqrt (Gibbs * Gibbs);

    if (mod != 0.0)
      Gibbs /= mod;

    // the modulus of the Gibbs vector = sinh(beta)
    if (beta)
      *beta = asinh (mod);

    return Gibbs;
  }
}

void Model::Boost::set_beta (double beta)
{
  if (get_nparam() == 1)
    set_param (0, beta);
 
  else {

    Vector<double, 3> Gibbs = get_axis();
    Gibbs *= sinh (beta);

    for (unsigned i=0; i<3; i++)
      set_param (i, Gibbs[i]);

  }
}

double Model::Boost::get_beta () const
{
  double beta;
  get_axis (&beta);
  return beta;
}


void Model::Boost::free_axis ()
{
  if (get_nparam() == 3)
    return;

  double beta = get_param (0);
  Vector<double, 3> Gibbs = axis * sinh(beta);

  resize (3);
  for (unsigned i=0; i<3; i++)
    set_param (i, Gibbs[i]);
}

//! Calculate the Jones matrix and its gradient
void Model::Boost::calculate (Jones<double>& result,
				    vector<Jones<double> >* grad)
{
  if (get_nparam() == 1)
    calculate_beta (result, grad);
  else
    calculate_Gibbs (result, grad);
}

//! Return the Jones matrix and its gradient
void Model::Boost::calculate_beta (Jones<double>& result,
					 vector<Jones<double> >* grad)
{
  double beta = get_param(0);

  if (verbose)
    cerr << "Model::Boost::calculate axis=" << axis 
	 << " beta=" << beta << endl;

  double sinh_beta = sinh (beta);
  double cosh_beta = cosh (beta);

  if (grad) {
    Quaternion<double, Hermitian> dboost_dbeta (sinh_beta, cosh_beta*axis);
    (*grad)[0] = convert (dboost_dbeta);

    if (verbose) {
      cerr << "Model::Boost::calculate gradient" << endl;
      cerr << "   " << (*grad)[0] << endl;
    }
  }

  Quaternion<double, Hermitian> boost (cosh_beta, sinh_beta*axis);

  result = convert (boost);
}



void Model::Boost::calculate_Gibbs (Jones<double>& result, 
					  vector<Jones<double> >* grad)
{
  Vector<double, 3> Gibbs;
  for (unsigned i=0; i<3; i++)
    Gibbs[i] = get_param(i);

  if (verbose)
    cerr << "Model::Boost::calculate Gibbs=" << Gibbs << endl;

  // calculate the Boost component
  double norm_Gibbs = Gibbs * Gibbs;
  
  double sinh_beta = sqrt (norm_Gibbs);
  double beta = asinh (sinh_beta);
  double cosh_beta = cosh (beta);
  
  // the Boost quaternion
  Quaternion<double, Hermitian> boost (cosh_beta, Gibbs);

  if (grad) {
    // build the partial derivatives with respect to boost Gibbs-vector
    double temp = 1.0 / sqrt (1.0 + norm_Gibbs);

    for (unsigned i=0; i<3; i++) {
      
      // partial derivative of Boost[0]=cosh(beta) wrt Boost[i+1]=boost[i]
      double dcosh_beta_dGibbsi = Gibbs[i] * temp;
      
      Quaternion<double, Hermitian> dboost_dGibbsi;
      dboost_dGibbsi[0] = dcosh_beta_dGibbsi;
      dboost_dGibbsi[i+1] = 1.0;

      // set the partial derivative wrt this parameter
      (*grad)[i] = convert (dboost_dGibbsi);

    }

    if (verbose) {
      cerr << "Model::Boost::calculate gradient" << endl;
      for (unsigned i=0; i<3; i++)
	cerr << "   " << (*grad)[i] << endl;
    }

  }

  result = convert (boost);
}


