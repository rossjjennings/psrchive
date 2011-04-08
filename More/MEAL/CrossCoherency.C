/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/CrossCoherency.h"
#include "MEAL/OrthogonalModes.h"
#include "MEAL/ModeCoherency.h"
#include "MEAL/UnitTangent.h"

#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ProjectGradient.h"

#include "Pauli.h"

using namespace std;

void MEAL::CrossCoherency::init ()
{
  ModeCoherency* A = dynamic_cast<ModeCoherency*>( modes->get_modeA() );

  axis = A->get_axis();
  composite.map (axis);

  ScalarMath betaA (A->get_beta());
  ScalarMath fluxA (A->get_intensity());
  ScalarMath Apos = fluxA * exp(betaA);
  ScalarMath Aneg = fluxA * exp(-betaA);

  ModeCoherency* B = dynamic_cast<ModeCoherency*>( modes->get_modeB() );
  ScalarMath betaB (B->get_beta());
  ScalarMath fluxB (B->get_intensity());
  ScalarMath Bpos = fluxB * exp(betaB);
  ScalarMath Bneg = fluxB * exp(-betaB);

  ScalarMath corrA1 = erf( *(new ScalarParameter) );
  ScalarMath fluxA1 = corrA1 * sqrt( Apos * Bneg );

  ScalarMath corrB1 = erf( *(new ScalarParameter) );
  ScalarMath fluxB1 = corrB1 * sqrt( Aneg * Bpos );

  stokes[0] = ( 2.0 * (fluxA1 + fluxB1) ).get_expression();
  stokes[1] = ( 2.0 * (fluxA1 - fluxB1) ).get_expression();

  ScalarMath fluxC = sqrt( Apos * Bpos ) + sqrt( Aneg * Bneg );

  ScalarMath corrC2 = erf( *(new ScalarParameter) );
  stokes[2] = ( 2.0 * corrC2 * fluxC ).get_expression();

  ScalarMath corrC3 = erf( *(new ScalarParameter) );
  stokes[3] = ( 2.0 * corrC3 * fluxC ).get_expression();

  for (unsigned i=0; i<4; i++)
    composite.map (stokes[i]);
}


MEAL::CrossCoherency::CrossCoherency (OrthogonalModes* _modes)
  : composite (this)
{
  modes = _modes;
  init ();
}

//! Copy constructor
MEAL::CrossCoherency::CrossCoherency (const CrossCoherency& copy)
  : composite (this)
{
  modes = new OrthogonalModes (*(copy.modes));
  init ();

  operator = (copy);
}

//! Assignment operator
MEAL::CrossCoherency&
MEAL::CrossCoherency::operator = (const CrossCoherency& that)
{
  if (this == &that)
    return *this;

  modes->copy( that.modes );

  for (unsigned i=0; i<4; i++)
    stokes[i]->copy( that.stokes[i] );

  return *this;
}

//! Destructor
MEAL::CrossCoherency::~CrossCoherency ()
{
}

//! Return the name of the class
string MEAL::CrossCoherency::get_name () const
{
  return "CrossCoherency";
}


//! Calculate the Jones matrix and its gradient
void MEAL::CrossCoherency::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  std::vector< Vector<3,double> > axis_gradient;
  Vector<3,double> unit = axis->evaluate( &axis_gradient );

  double stokes_value[4];
  std::vector<double> stokes_gradient [4];

  Vector<3,double> sum;

  for (unsigned i=0; i<4; i++)
  {
    std::vector<double>* stokes_gradptr = stokes_gradient + i;
    if (!grad)
      stokes_gradptr = 0;
    
    stokes_value[i] = stokes[i]->evaluate( stokes_gradptr );

    cerr << "S[" << i << "]=" << stokes_value[i] << endl;

    if (i == 1)
      sum += stokes_value[i] * unit;
    else if (i > 1)
      sum += stokes_value[i] * axis_gradient[i-2];
  }

  // the Boost quaternion
  Quaternion<double, Hermitian> Hboost (stokes[0], sum);

  result = convert (Hboost);

  if (!grad)
    return;

  unsigned nparam = get_nparam();
  
  grad->resize (nparam);
  for (unsigned iparam=0; iparam<nparam; iparam++)
    (*grad)[iparam] = 0.0;

  for (unsigned i=0; i < 4; i++)
  {
    // partial derivatives with respect to stokes[i] parameters
    vector< Jones<double> > comp_gradient( stokes_gradient[i].size() );

    for (unsigned iparam=0; iparam < stokes_gradient[i].size(); iparam++)
    {
      Quaternion<double, Hermitian> grad;

      if (i == 0)
	grad[0] = stokes_gradient[i][iparam];
      else if (i == 1)
	grad.set_vector( stokes_gradient[i][iparam] * unit );
      else
	grad.set_vector( stokes_gradient[i][iparam] * axis_gradient[i-2] );

      comp_gradient[iparam] = convert(grad);
    }

    // re-map the elements of the component gradient into the Composite space
    ProjectGradient (stokes[i], comp_gradient, *grad);
  }

  // partial derivatives with respect to unit vector
  vector< Jones<double> > comp_gradient( axis_gradient.size() );

  for (unsigned iparam=0; iparam < axis_gradient.size(); iparam++)
  {
    Quaternion<double, Hermitian> grad;
    grad.set_vector (stokes[1]*axis_gradient[iparam] - stokes[2+iparam]*unit);
    comp_gradient[iparam] = convert(grad);
  }

  // re-map the elements of the component gradient into the Composite space
  ProjectGradient (axis, comp_gradient, *grad);
  
  if (verbose) {
    cerr << "MEAL::BoostUnion::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << (*grad)[i] << endl;
  }

}
