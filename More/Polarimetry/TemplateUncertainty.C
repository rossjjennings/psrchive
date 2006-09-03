/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/TemplateUncertainty.h"

#include <iostream>


//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
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
  template_variance.set_variance (v);
  built = false;
}

Stokes<double> Calibration::TemplateUncertainty::get_input_var () const
{
  return observation_variance + template_variance.get_input_variance();
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
  template_variance.set_transformation( transformation->evaluate() );

  Stokes<double> var = template_variance.get_variance();

  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_var[ipol] = 1.0 / (observation_variance[ipol] + var[ipol]);

#if 0
  std::cerr << "Calibration::TemplateUncertainty::build"
	    << "\n  tran_var=" << var
	    << "\n  obs_var=" << observation_variance[0] 
	    << "\n  inv_var=" << inv_var << std::endl;
#endif

  built = true;
}
