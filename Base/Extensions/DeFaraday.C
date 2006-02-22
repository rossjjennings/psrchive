#include "Pulsar/DeFaraday.h"
#include "Physical.h"

//! Default constructor
Pulsar::DeFaraday::DeFaraday ()
  : Extension ("Example")
{
  rotation_measure = 0.0;
  reference_wavelength = 0.0;
}

//! Copy constructor
Pulsar::DeFaraday::DeFaraday (const DeFaraday& extension)
  : Extension ("Example")
{
  operator=(extension);
}

//! Operator =
const Pulsar::DeFaraday&
Pulsar::DeFaraday::operator= (const DeFaraday& extension)
{
  rotation_measure = extension.rotation_measure;
  reference_wavelength = extension.reference_wavelength;

  return *this;
}

//! Destructor
Pulsar::DeFaraday::~DeFaraday ()
{
}

//! Set the rotation measure
void Pulsar::DeFaraday::set_rotation_measure (double rm)
{
  rotation_measure = rm;
}

//! Get the rotation measure
double Pulsar::DeFaraday::get_rotation_measure () const
{
  return rotation_measure;
}

//! Set the reference wavelength in metres
void Pulsar::DeFaraday::set_reference_wavelength (double metres)
{
  reference_wavelength = metres;
}

//! Get the reference wavelength
double Pulsar::DeFaraday::get_reference_wavelength () const
{
  return reference_wavelength;
}

//! Set the reference frequency in MHz
void Pulsar::DeFaraday::set_reference_frequency (double MHz)
{
  reference_wavelength = speed_of_light / (MHz * 1e6);
}

//! Get the reference frequency in MHz
double Pulsar::DeFaraday::get_reference_frequency () const
{
  return 1e-6 * speed_of_light / reference_wavelength;
}
