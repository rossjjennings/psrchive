#include "Pulsar/CalibratorExtension.h"
#include "Calibration/SingleAxis.h"
#include "Calibration/Instrument.h"
#include "Calibration/Polar.h"

//! Default constructor
Pulsar::CalibratorExtension::CalibratorExtension ()
{
  type = SingleAxis;
}

//! Copy constructor
Pulsar::CalibratorExtension::CalibratorExtension (const CalibratorExtension& c)
{
  operator = (c);
}

//! Operator =
const Pulsar::CalibratorExtension&
Pulsar::CalibratorExtension::operator= (const CalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  type = copy.get_type();
  
  unsigned nchan = copy.get_nchan();

  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    *(response[ichan]) = *(copy.response[ichan]);

  return *this;
}

//! Destructor
Pulsar::CalibratorExtension::~CalibratorExtension ()
{
}

//! Set the type of the instrumental response parameterization
void Pulsar::CalibratorExtension::set_type (CalibratorType _type)
{
  if (type == _type)
    return;

  type = _type;
  construct ();
}

//! Get the type of the instrumental response parameterization
Pulsar::CalibratorType Pulsar::CalibratorExtension::get_type () const
{
  return type;
}

//! Set the name of the instrumental response parameterization
void Pulsar::CalibratorExtension::set_name (const string& name)
{
  throw Error (InvalidState, "Pulsar::CalibratorExtension::set_name",
	       "not implemented");
}

//! Get the name of the instrumental response parameterization
string Pulsar::CalibratorExtension::get_name () const
{
  throw Error (InvalidState, "Pulsar::CalibratorExtension::get_name",
	       "not implemented");
}


//! Set the number of frequency channels
void Pulsar::CalibratorExtension::set_nchan (unsigned _nchan)
{
  if (response.size() == _nchan)
    return;

  response.resize( _nchan );
  construct ();
}

//! Get the number of frequency channels
unsigned Pulsar::CalibratorExtension::get_nchan () const
{
  return response.size();
}

//! Get the transformation for the specified frequency channel
::Calibration::Transformation* 
Pulsar::CalibratorExtension::get_Transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::CalibratorExtension::get_Transformation");
  return response[ichan];
}

//! Get the transformation for the specified frequency channel
const ::Calibration::Transformation*
Pulsar::CalibratorExtension::get_Transformation (unsigned ichan) const
{
  range_check (ichan, "Pulsar::CalibratorExtension::get_Transformation");
  return response[ichan];
}

void Pulsar::CalibratorExtension::construct ()
{
  for (unsigned ichan=0; ichan<response.size(); ichan++)
    response[ichan] = new_Transformation();
}

void Pulsar::CalibratorExtension::range_check (unsigned ichan,
					       const char* method) const
{
  if (ichan >= response.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
		 ichan, response.size());
}

//! Return a new Transformation instance, based on type attribute
::Calibration::Transformation* 
Pulsar::CalibratorExtension::new_Transformation ()
{
  switch (type) {
  case SingleAxis:
    return new ::Calibration::SingleAxis;
  case Polar:
    return new ::Calibration::Polar;
  case Hamaker:
    return new ::Calibration::Polar;
  case Britton:
    return new ::Calibration::Instrument;
  default:
    throw Error (InvalidState,
		 "Pulsar::CalibratorExtension::new_Transformation",
		 "unrecognized CalibratorType = %d", (int) type);
  }
}

