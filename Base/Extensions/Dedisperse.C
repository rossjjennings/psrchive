/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Dedisperse.h"
#include "Physical.h"

//! Default constructor
Pulsar::Dedisperse::Dedisperse ()
  : Extension ("Dedisperse")
{
  dispersion_measure = 0.0;
  reference_wavelength = 0.0;
}

//! Copy constructor
Pulsar::Dedisperse::Dedisperse (const Dedisperse& extension)
  : Extension ("Dedisperse")
{
  operator=(extension);
}

//! Operator =
const Pulsar::Dedisperse&
Pulsar::Dedisperse::operator= (const Dedisperse& extension)
{
  dispersion_measure = extension.dispersion_measure;
  reference_wavelength = extension.reference_wavelength;

  return *this;
}

//! Destructor
Pulsar::Dedisperse::~Dedisperse ()
{
}

//! Set the dispersion measure
void Pulsar::Dedisperse::set_dispersion_measure (double rm)
{
  dispersion_measure = rm;
}

//! Get the dispersion measure
double Pulsar::Dedisperse::get_dispersion_measure () const
{
  return dispersion_measure;
}

//! Set the reference wavelength in metres
void Pulsar::Dedisperse::set_reference_wavelength (double metres)
{
  reference_wavelength = metres;
}

//! Get the reference wavelength
double Pulsar::Dedisperse::get_reference_wavelength () const
{
  return reference_wavelength;
}

//! Set the reference frequency in MHz
void Pulsar::Dedisperse::set_reference_frequency (double MHz)
{
  reference_wavelength = speed_of_light / (MHz * 1e6);
}

//! Get the reference frequency in MHz
double Pulsar::Dedisperse::get_reference_frequency () const
{
  return 1e-6 * speed_of_light / reference_wavelength;
}
