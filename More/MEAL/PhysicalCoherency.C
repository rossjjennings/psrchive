/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/PhysicalCoherency.h"
#include "MEAL/Parameters.h"
#include "Pauli.h"
#include "Error.h"

using namespace std;

MEAL::PhysicalCoherency::PhysicalCoherency ()
{
  Parameters* parameters = new Parameters (this, 4);

  parameters->set_param_name (0, "log(I-p)");
  parameters->set_param_name (1, "StokesQ");
  parameters->set_param_name (2, "StokesU");
  parameters->set_param_name (3, "StokesV");
}

//! Return the name of the class
string MEAL::PhysicalCoherency::get_name () const
{
  return "PhysicalCoherency";
}


//! Calculate the Jones matrix and its gradient
void MEAL::PhysicalCoherency::calculate (Jones<double>& result,
					 std::vector<Jones<double> >* grad)
{

  Stokes<double> stokes;

  for (unsigned i=1; i<4; i++)
    stokes[i] = get_param (i);

  double unpolarized_flux = exp(get_param(0));
  double polarized_flux = stokes.abs_vect();

  stokes[0] = polarized_flux + unpolarized_flux;

  result = convert(stokes);

  if (grad) {

    grad->resize (4);

    Stokes<double> unpol (1,0,0,0);
    Jones<double> identity = convert (unpol);

    (*grad)[0] = unpolarized_flux * identity;

    for (unsigned i=1; i<4; i++) {
      Stokes<double> param;
      param[i] = 1.0;
      (*grad)[i] = convert (param) + stokes[i]/polarized_flux * identity;
    }

  }

  if (verbose) {
    cerr << "MEAL::PhysicalCoherency::get_stokes result\n"
      "   " << result << endl;
    if (grad) {
      cerr << "MEAL::PhysicalCoherency::get_stokes gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

//! Set the Stokes parameters of the model
void MEAL::PhysicalCoherency::set_stokes (const Stokes<double>& stokes)
{
  for (unsigned i=1; i<4; i++)
    set_param (i, stokes[i]);

  set_param (0, log(stokes[0]-stokes.abs_vect()));
}

//! Set the Stokes parameters of the model
void MEAL::PhysicalCoherency::set_stokes (const Stokes<Estimate<double> >& s)
{
  for (unsigned i=1; i<4; i++) {
    set_param (i, s[i].val);
    set_variance (i, s[i].var);
  }

  Estimate<double> log_unpol = log (s[0]-s.abs_vect());
  set_param (0, log_unpol.val);
  set_variance (0, log_unpol.var);
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > MEAL::PhysicalCoherency::get_stokes () const
{
  Stokes< Estimate<double> > stokes;
  for (unsigned i=1; i<4; i++)
    stokes[i] = get_Estimate (i);

  stokes[0] = stokes.abs_vect() + exp(get_Estimate(0));

  return stokes;
}
