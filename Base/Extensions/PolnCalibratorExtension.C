#include "Pulsar/PolnCalibratorExtension.h"

#include "Calibration/SingleAxis.h"
#include "Calibration/Instrument.h"
#include "Calibration/Polar.h"

//! Default constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension ()
  : Extension ("PolnCalibratorExtension")
{
  type = Calibrator::SingleAxis;
}

//! Copy constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibratorExtension& copy)
  : Extension ("PolnCalibratorExtension")
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
  epoch = copy.get_epoch();

  unsigned nchan = copy.get_nchan();

  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( copy.get_valid(ichan) )
      response[ichan]->copy(copy.response[ichan]);
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

MJD Pulsar::PolnCalibratorExtension::get_epoch () const
{
  return epoch;
}

void Pulsar::PolnCalibratorExtension::set_epoch (const MJD& _epoch)
{
  epoch = _epoch;
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

void Pulsar::PolnCalibratorExtension::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::set_valid");

  if (!valid)
    response[ichan] = 0;
  else if (!response[ichan])
    response[ichan] = new_transformation ();
}


//! Get the transformation for the specified frequency channel
::Calibration::Complex2* 
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return response[ichan];
}

//! Get the transformation for the specified frequency channel
const ::Calibration::Complex2*
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return response[ichan];
}

void Pulsar::PolnCalibratorExtension::construct ()
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::PolnCalibratorExtension::construct nchan="
         << response.size() << " type=" 
         << Calibrator::Type2str (get_type()) << endl;

  for (unsigned ichan=0; ichan<response.size(); ichan++)
    response[ichan] = new_transformation();

}

void Pulsar::PolnCalibratorExtension::range_check (unsigned ichan,
						   const char* method) const
{
  if (ichan >= response.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
		 ichan, response.size());
}

//! Return a new Calibration::Complex2 instance, based on type attribute
::Calibration::Complex2* 
Pulsar::PolnCalibratorExtension::new_transformation ()
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
		 "Pulsar::PolnCalibratorExtension::new_transformation",
		 "unrecognized Calibrator::Type = %d", (int) type);
  }
}


const char* Pulsar::Calibrator::Type2str (Type type)
{
  switch (type) {
  case Flux:
    return "Flux";
  case SingleAxis:
    return "SingleAxis";
  case Polar:
    return "Polar";
  case Hamaker:
    return "Hamaker";
  case Britton:
    return "Britton";
  default:
    return "Invalid";
  }
}


Pulsar::Calibrator::Type Pulsar::Calibrator::str2Type (const char* s)
{
  if (strcasecmp(s, "Flux") == 0)
    return Flux;
  if (strcasecmp(s, "SingleAxis") == 0)
    return SingleAxis;
  if (strcasecmp(s, "Polar") == 0)
    return Polar;
  if (strcasecmp(s, "Hamaker") == 0)
    return Hamaker;
  if (strcasecmp(s, "Britton") == 0)
    return Britton;
  return (Type) -1;
}

