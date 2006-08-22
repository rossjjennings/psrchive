/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/TemplateUncertainty.h"
#include "Pauli.h"

#include <iostream>

template<class T>
class public_complex {
public:
  T real;
  T imag;
};

template<class T>
public_complex<T>* pub (std::complex<T>& c)
{
  return reinterpret_cast<public_complex<T>*>( &c );
}

//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
  Jones<MEAL::ScalarMath> J;

  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++) {
      pub(J(i,j))->real = pub(xform(i,j))->real;
      pub(J(i,j))->imag = pub(xform(i,j))->imag;
    }

  output = transform (input, J);

  built = false;
}

//! Return the inverse of the variance of the specified polarization
double Calibration::TemplateUncertainty::get_inv_var (unsigned ipol) const
{
  if (!built)
    const_cast<TemplateUncertainty*>(this)->build();

  return inv_var [ipol];
}
    
//! Set the uncertainty of the observation
void
Calibration::TemplateUncertainty::set_observation_var (const Stokes<double>& v)
{
  observation_variance = v;
  built = false;
}

//! Set the uncertainty of the template
void
Calibration::TemplateUncertainty::set_template_var (const Stokes<double>& v)
{
  for (unsigned ipol=0; ipol < 4; ipol++) {
    template_variance[ipol].var = v[ipol];
    input[ipol].get_expression()->set_Estimate( 0, template_variance[ipol] );
  }

  built = false;
}

//! Set the transformation from template to observation
void
Calibration::TemplateUncertainty::set_transformation (const MEAL::Complex2* x)
{
  if (transformation)
    transformation->changed.disconnect (this, &TemplateUncertainty::changed);

  transformation = const_cast<MEAL::Complex2*>(x);
  transformation->changed.connect (this, &TemplateUncertainty::changed);

  built = false;
}

void Calibration::TemplateUncertainty::changed (MEAL::Function::Attribute a)
{
  if (a == MEAL::Function::Evaluation)
    built = false;
}

#include <iostream>
using namespace std;

//! Useful for quickly printing the values
inline std::ostream&
operator << (std::ostream& o, const MEAL::Scalar& sm)
{
  return o << sm.evaluate();
}

void Calibration::TemplateUncertainty::build ()
{
  cerr << "XXX " << output[0].get_expression()->get_nparam() << endl;

  Jones<double> J = transformation->evaluate();

  for (unsigned i=0; i<2; i++)
    for (unsigned j=0; j<2; j++) {
      pub(xform(i,j))->real.set_value( pub(J(i,j))->real );
      pub(xform(i,j))->imag.set_value( pub(J(i,j))->imag );
    }

  Stokes<double> var;
  MEAL::Function::verbose = true;
  for (unsigned ipol=0; ipol< 4; ipol++)
    var[ipol] = output[ipol].get_Estimate().get_variance();
  MEAL::Function::verbose = false;

  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_var[ipol] = 1.0 / (observation_variance[ipol] + var[ipol]);

#ifdef _DEBUG
  std::cerr << "Calibration::TemplateUncertainty::build"
	    << "\n  temp_var=" << template_variance[0].var
	    << "\n  tran_var=" << var[0].var
	    << "\n  obs_var=" << observation_variance[0] 
	    << "\n  inv_var=" << inv_var << std::endl;
#endif

  built = true;
}
