/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/UnitTangent.h"
#include "MEAL/Parameters.h"

using namespace std;

MEAL::UnitTangent::UnitTangent ()
{
  // a unit vector in three dimensions has two degrees of freedom
  new Parameters (this, 2);

  for (unsigned i=0; i<3; i++)
    basis[i] = Vector<3,double>::basis(i);
}

//! Return the name of the class
string MEAL::UnitTangent::get_name () const
{
  return "UnitTangent";
}

void MEAL::UnitTangent::set_vector (Vector<3,double> direction)
{
  // find the most distant axis
  unsigned b0 = 0;
  for (unsigned i=1; i<3; i++)
    if ( fabs(direction[i]) < fabs(direction[b0]) )
      b0 = i;

  // the other two axes
  unsigned b1 = (b0+1)%3;
  unsigned b2 = (b0+2)%3;

  // the squared magnitude of the vector in the plane perpendicular to b0
  double perp_sq = direction[b1]*direction[b1] + direction[b2]*direction[b2];

  // the magnitude of the vector
  double norm = sqrt( direction[b0]*direction[b0] + perp_sq );

  // the unit vector
  direction /= norm;

  basis[0] = direction;

  // phi is the angle between direction and the most distant axis
  // sin = opposite / hypotenuse = perp / norm
  double sin_phi = sqrt(perp_sq) / norm;
  double cos_phi = direction[b0];

  // basis[1] points along the meridian to the most distant axis
  basis[1] = direction;
  basis[1][b0] = sin_phi;
  basis[1][b1] *= -cos_phi/sin_phi;
  basis[1][b2] *= -cos_phi/sin_phi;

  // basis[2] is perpendicular to the first two
  basis[2][b0] = 0;
  basis[2][b1] = -direction[b2];
  basis[2][b2] = direction[b1];

  set_param (0, 0.0);
  set_param (1, 0.0);
}

//! Calculate the Jones matrix and its gradient
void MEAL::UnitTangent::calculate (Vector<3,double>& result,
				   vector<Vector<3,double> >* grad)
{
  double del0 = get_param(0);
  double del1 = get_param(1);

  if (del0 != 0 || del1 != 0)
  {
    if (verbose)
      cerr << "MEAL::UnitTangent::calculate del0=" << del0 << " del1=" << del1
	   << endl;

    set_vector( basis[0] + del0*basis[1] + del1*basis[2] );
  }

  result = basis[0];

  if (!grad)
    return;
  
  for (unsigned i=0; i<2; i++)
    (*grad)[i] = basis[i+1];
  
  if (verbose)
  {
    cerr << "MEAL::UnitTangent::calculate gradient" << endl;
    for (unsigned i=0; i<2; i++)
      cerr << "   " << (*grad)[i] << endl;
  }
}


