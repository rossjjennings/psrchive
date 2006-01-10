#include "Calibration/TemplateUncertainty.h"
#include "Pauli.h"

#include <iostream>

//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
  built = false;
}

//! Return the inverse of the variance of the specified polarization
float Calibration::TemplateUncertainty::get_inv_var (unsigned ipol) const
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
  for (unsigned ipol=0; ipol < 4; ipol++)
    template_variance[ipol].var = v[ipol];
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

void Calibration::TemplateUncertainty::build ()
{
  Stokes< Estimate<double> > var
    = transform (template_variance, transformation->evaluate());

  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_var[ipol] = 1.0 / (observation_variance[ipol] + var[ipol].var);

#ifdef _DEBUG
  std::cerr << "Calibration::TemplateUncertainty::build"
	    << "\n  temp_var=" << template_variance[0].var
	    << "\n  tran_var=" << var[0].var
	    << "\n  obs_var=" << observation_variance[0] 
	    << "\n  inv_var=" << inv_var << std::endl;
#endif

  built = true;
}
