/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/SingularCoherency.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"
#include "Error.h"

using namespace std;

MEAL::SingularCoherency::SingularCoherency ()
{
  Parameters* parameters = new Parameters (this, 3);

  parameters->set_name (0, "StokesQ");
  parameters->set_name (1, "StokesU");
  parameters->set_name (2, "StokesV");
}

//! Return the name of the class
string MEAL::SingularCoherency::get_name () const
{
  return "SingularCoherency";
}


//! Calculate the Jones matrix and its gradient
void MEAL::SingularCoherency::calculate (Jones<double>& result,
					 std::vector<Jones<double> >* grad)
{
  // unpack p = (Q, U, V)
  Vector<3,double> p;
  for (unsigned i=0; i<3; i++)
    p[i] = get_param (i);

  // I = |p|
  Stokes<double> stokes( norm(p), p );
  result = convert(stokes);

  if (verbose)
    cerr << "MEAL::SingularCoherency::get_stokes result\n"
      "   " << result << endl;

  if (!grad)
    return;

  grad->resize (3);

  for (unsigned i=0; i<3; i++)
  {
    double dI_dSi = p[i];
    if (stokes[0] != 0)
      dI_dSi /= stokes[0];

    Stokes<double> dS_dSi;
    dS_dSi[0] = dI_dSi;
    dS_dSi[i+1] = 1.0;

    if (verbose)
      cerr << "dS_dS" << i << "=" << dS_dSi << endl;

    (*grad)[i] = convert (dS_dSi);
  }

  if (verbose)
  {
    cerr << "MEAL::SingularCoherency::get_stokes gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}

//! Set the Stokes parameters of the model
void MEAL::SingularCoherency::set_stokes (const Stokes<double>& stokes)
{
  set_vector( stokes.get_vector() );
}

void MEAL::SingularCoherency::set_vector (const Vector<3,double>& v)
{
  for (unsigned i=0; i<3; i++)
    set_param (i, v[i]);
}



//! Set the Stokes parameters of the model
void MEAL::SingularCoherency::set_stokes (const Stokes<Estimate<double> >& s)
{
  for (unsigned i=0; i<3; i++)
    set_Estimate (i, s[i+1]);
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::SingularCoherency::get_stokes () const
{
  Vector< 3, Estimate<double> > p;
  for (unsigned i=0; i<3; i++)
    p[i] = get_Estimate (i);

  return Stokes< Estimate<double> > (norm(p), p);
}
