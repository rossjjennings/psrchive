#include "Pulsar/PolnCalibratorExtension.h"

#include "Calibration/SingleAxis.h"
#include "Calibration/Instrument.h"
#include "Calibration/Polar.h"

//! Default constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension ()
{
  type = Calibrator::SingleAxis;
}

//! Copy constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibratorExtension& copy)
{
  operator = (copy);
}

//! Operator =
const Pulsar::PolnCalibratorExtension&
Pulsar::PolnCalibratorExtension::operator= 
(const PolnCalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  type = copy.get_type();
  
  unsigned nchan = copy.get_nchan();

  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( copy.get_valid(ichan) )
      *(response[ichan]) = *(copy.response[ichan]);
    else
      response[ichan] = 0;

  return *this;
}

//! Destructor
Pulsar::PolnCalibratorExtension::~PolnCalibratorExtension ()
{
}

//! Set the type of the instrumental response parameterization
void Pulsar::PolnCalibratorExtension::set_type (Calibrator::Type _type)
{
  if (type == _type)
    return;

  type = _type;
  construct ();
}

//! Get the type of the instrumental response parameterization
Pulsar::Calibrator::Type Pulsar::PolnCalibratorExtension::get_type () const
{
  return type;
}

//! Set the number of frequency channels
void Pulsar::PolnCalibratorExtension::set_nchan (unsigned _nchan)
{
  if (response.size() == _nchan)
    return;

  response.resize( _nchan );
  construct ();
}

//! Get the number of frequency channels
unsigned Pulsar::PolnCalibratorExtension::get_nchan () const
{
  return response.size();
}

bool Pulsar::PolnCalibratorExtension::get_valid (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_valid");
  return response[ichan];
}

//! Get the transformation for the specified frequency channel
::Calibration::Transformation* 
Pulsar::PolnCalibratorExtension::get_Transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_Transformation");
  return response[ichan];
}

//! Get the transformation for the specified frequency channel
const ::Calibration::Transformation*
Pulsar::PolnCalibratorExtension::get_Transformation (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_Transformation");
  return response[ichan];
}

void Pulsar::PolnCalibratorExtension::construct ()
{
  for (unsigned ichan=0; ichan<response.size(); ichan++)
    response[ichan] = new_Transformation();
}

void Pulsar::PolnCalibratorExtension::range_check (unsigned ichan,
					       const char* method) const
{
  if (ichan >= response.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
		 ichan, response.size());
}

//! Return a new Transformation instance, based on type attribute
::Calibration::Transformation* 
Pulsar::PolnCalibratorExtension::new_Transformation ()
{
  switch (type) {
  case Calibrator::SingleAxis:
    return new ::Calibration::SingleAxis;
  case Calibrator::Polar:
    return new ::Calibration::Polar;
  case Calibrator::Hamaker:
    return new ::Calibration::Polar;
  case Calibrator::Britton:
    return new ::Calibration::Instrument;
  default:
    throw Error (InvalidState,
		 "Pulsar::PolnCalibratorExtension::new_Transformation",
		 "unrecognized Calibrator::Type = %d", (int) type);
  }
}

