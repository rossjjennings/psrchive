/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TemplateUncertainty.h"
#include "MEAL/StokesError.h"

#include <iostream>
using namespace std;

complex<double> reim (double s)
{
  return complex<double> (s,s);
}

Stokes< complex<double> > duplicate (const Stokes<double>& s)
{
  return Stokes< complex<double> > (s, reim);
}

//! Default constructor
Calibration::TemplateUncertainty::TemplateUncertainty ()
{
  built = false;
}

Calibration::TemplateUncertainty*
Calibration::TemplateUncertainty::clone () const
{
  return new TemplateUncertainty (*this);
}

//! Set the uncertainty of the observation
void Calibration::TemplateUncertainty::set_variance
( const Stokes<double>& var )
{
  observation_variance = duplicate (var);
  built = false;
}

//! Set the uncertainty of the template
void Calibration::TemplateUncertainty::set_template_variance
( const Stokes<double>& var )
{
  template_variance = duplicate (var);
  built = false;
}

//! Set the uncertainty of the observation
void Calibration::TemplateUncertainty::set_variance
( const Stokes< complex<double> >& var )
{
  observation_variance = var;
  built = false;
}

//! Set the uncertainty of the template
void Calibration::TemplateUncertainty::set_template_variance
(const Stokes< complex<double> >& var)
{
  template_variance = var;
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
  compute.set_transformation( transformation->evaluate() );

  Stokes<double> re = real( template_variance );
  Stokes<double> im = imag( template_variance );

#ifdef _DEBUG
  cerr << "var=" << template_variance << endl;
  cerr << "re=" << re << endl;
  cerr << "im=" << im << endl;
#endif

  compute.set_variance (re);
  re = compute.get_variance();

  compute.set_variance (im);
  im = compute.get_variance();

  Stokes< complex<double> > variance;
  for (unsigned i=0; i<4; i++)
    variance[i] = complex<double>( re[i], im[i] );

  variance += observation_variance;

  ObservationUncertainty::set_variance (variance);

  built = true;
}

