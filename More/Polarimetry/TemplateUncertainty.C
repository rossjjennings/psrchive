/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/TemplateUncertainty.h"
#include "MEAL/StokesError.h"

#include <iostream>
using namespace std;

//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
  built = false;
}


//! Set the uncertainty of the observation
void
Calibration::TemplateUncertainty::set_variance (const Stokes<double>& v)
{
  observation_variance = v;
  built = false;
}

//! Set the uncertainty of the template
void Calibration::TemplateUncertainty::set_template_variance
(const Stokes<double>& v)
{
  template_variance = v;
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


//! Given a coherency matrix, return the difference
double Calibration::TemplateUncertainty::get_weighted_norm
(const Jones<double>& matrix) const
{
  if (!built)
    const_cast<TemplateUncertainty*>(this)->build();

  return ObservationUncertainty::get_weighted_norm (matrix);
}

//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::TemplateUncertainty::get_weighted_conjugate
(const Jones<double>& matrix) const
{
  if (!built)
    const_cast<TemplateUncertainty*>(this)->build();

  return ObservationUncertainty::get_weighted_conjugate (matrix);
}

void Calibration::TemplateUncertainty::changed (MEAL::Function::Attribute a)
{
  if (a == MEAL::Function::Evaluation)
    built = false;
}

void Calibration::TemplateUncertainty::build ()
{
  MEAL::StokesError compute;

  compute.set_variance( template_variance );
  compute.set_transformation( transformation->evaluate() );

  Stokes<double> var = compute.get_variance();

  for (unsigned ipol=0; ipol < 4; ipol++)
    inv_variance[ipol] = 1.0 / (observation_variance[ipol] + var[ipol]);

#if 0
  std::cerr << "Calibration::TemplateUncertainty::build"
	    << "\n  tran_var=" << var
	    << "\n  obs_var=" << observation_variance
	    << "\n  inv_var=" << inv_variance << std::endl;
#endif

  built = true;
}
