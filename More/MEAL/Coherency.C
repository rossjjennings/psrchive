#include "MEPL/Coherency.h"
#include "Pauli.h"
#include "Error.h"

Model::Coherency::Coherency ()
  : OptimizedComplex2 (4)
{
}

//! Return the name of the class
string Model::Coherency::get_name () const
{
  return "Coherency";
}

//! Return the name of the specified parameter
string Model::Coherency::get_param_name (unsigned index) const
{
  switch (index) {
  case 0:
    return "StokesI";
  case 1:
    return "StokesQ";
  case 2:
    return "StokesU";
  case 3:
    return "StokesV";
  default:
    return "ERROR";
  }
}

//! Calculate the Jones matrix and its gradient
void Model::Coherency::calculate (Jones<double>& result,
					vector<Jones<double> >* grad)
{

  Stokes<double> stokes;
  for (unsigned i=0; i<4; i++)
    stokes[i] = get_param (i);

  result = convert(stokes);

  if (grad) {

    grad->resize (4);

    for (unsigned i=0; i<4; i++) {
      Stokes<double> param;
      param[i] = 1.0;
      (*grad)[i] = convert (param);
    }

  }

  if (verbose) {
    cerr << "Model::Coherency::get_stokes result\n"
      "   " << result << endl;
    if (grad) {
      cerr << "Model::Coherency::get_stokes gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

//! Set the Stokes parameters of the model
void Model::Coherency::set_stokes (const Stokes<double>& stokes)
{
  for (unsigned i=0; i<4; i++)
    set_param (i, stokes[i]);
}

//! Set the Stokes parameters of the model
void Model::Coherency::set_stokes (const Stokes<Estimate<double> >& s)
{
  for (unsigned i=0; i<4; i++) {
    set_param (i, s[i].val);
    set_variance (i, s[i].var);
  }
}

//! Set the Stokes parameters of the model
Stokes< Estimate<double> > Model::Coherency::get_stokes () const
{
  Stokes< Estimate<double> > stokes;
  for (unsigned i=0; i<4; i++)
    stokes[i] = get_Estimate (i);

  return stokes;
}
