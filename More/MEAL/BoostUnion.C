/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/BoostUnion.h"
#include "MEAL/ProjectGradient.h"
#include "Pauli.h"

using namespace std;

MEAL::BoostUnion::BoostUnion () : composite (this)
{
}

//! Return the name of the class
string MEAL::BoostUnion::get_name () const
{
  return "BoostUnion";
}

//! Set the model that defines the unit vector
void MEAL::BoostUnion::set_axis (Evaluable< Vector<3,double> >* _axis)
{
  if (axis)
    composite.unmap( axis );

  axis = _axis;

  composite.map( axis );
}

//! Set the model that defines the unit vector
void MEAL::BoostUnion::set_beta (Evaluable< double >* _beta)
{
  if (beta)
    composite.unmap( beta );

  beta = _beta;

  composite.map( beta );
}

//! Get the unit-vector along which the boost occurs
Vector<3, double> MEAL::BoostUnion::get_axis () const
{
  if (!axis)
    throw Error (InvalidState, "MEAL::BoostUnion::get_axis",
		 "axis model not defined");

  return axis->evaluate();
}

double MEAL::BoostUnion::get_beta () const
{
  if (!beta)
    throw Error (InvalidState, "MEAL::BoostUnion::get_beta",
		 "beta model not defined");

  return beta->evaluate ();
}

//! Calculate the Jones matrix and its gradient
void MEAL::BoostUnion::calculate (Jones<double>& result,
				 vector<Jones<double> >* grad)
{
  std::vector< Vector<3,double> > axis_gradient;
  std::vector< Vector<3,double> >* axis_gradptr = &axis_gradient;
  if (!grad)
    axis_gradptr = 0;

  Vector<3,double> unit = axis->evaluate( axis_gradptr );

  std::vector<double> beta_gradient;
  std::vector<double>* beta_gradptr = &beta_gradient;
  if (!grad)
    beta_gradptr = 0;

  double boost = beta->evaluate( beta_gradptr );

  double cosh_beta = cosh (boost);
  double sinh_beta = sinh (boost);

  if (verbose)
    cerr << "MEAL::BoostUnion::calculate beta=" << boost << endl;

  // the Boost quaternion
  Quaternion<double, Hermitian> Hboost (cosh_beta, sinh_beta*unit);

  result = convert (Hboost);

  if (!grad)
    return;

  unsigned nparam = get_nparam();
  
  grad->resize (nparam);
  for (unsigned iparam=0; iparam<nparam; iparam++)
    (*grad)[iparam] = 0.0;

  // partial derivatives with respect to scalar beta
  vector< Jones<double> > comp_gradient( beta_gradient.size() );

  for (unsigned iparam=0; iparam < beta_gradient.size(); iparam++)
  {
    Quaternion<double, Hermitian> bgrad (sinh_beta*beta_gradient[iparam],
					 cosh_beta*beta_gradient[iparam]*unit);
    comp_gradient[iparam] = convert(bgrad);
  }

  // re-map the elements of the component gradient into the Composite space
  ProjectGradient (beta, comp_gradient, *grad);


  // partial derivatives with respect to unit vector
  comp_gradient.resize( axis_gradient.size() );

  for (unsigned iparam=0; iparam < axis_gradient.size(); iparam++)
  {
    Quaternion<double, Hermitian> agrad (0.0, sinh_beta*axis_gradient[iparam]);
    comp_gradient[iparam] = convert(agrad);
  }

  // re-map the elements of the component gradient into the Composite space
  ProjectGradient (axis, comp_gradient, *grad);
  
  if (verbose)
  {
    cerr << "MEAL::BoostUnion::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << (*grad)[i] << endl;
  }

}


