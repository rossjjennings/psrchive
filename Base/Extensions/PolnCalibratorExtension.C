/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnCalibratorExtension.h"

#include "Calibration/SingleAxis.h"
#include "Calibration/Instrument.h"
#include "MEAL/Polar.h"

//! Default constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension ()
  : CalibratorExtension ("PolnCalibratorExtension")
{
  type = Calibrator::SingleAxis;
  nparam = 0;
}

//! Copy constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibratorExtension& copy)
  : CalibratorExtension (copy)
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
  nparam = copy.get_nparam();

  unsigned nchan = copy.get_nchan();

  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( copy.get_valid(ichan) )
      response[ichan]->copy(copy.response[ichan]);
    else
      set_valid (ichan, false);

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
  nparam = 0;
  construct ();
}


//! Set the number of frequency channels
void Pulsar::PolnCalibratorExtension::set_nchan (unsigned _nchan)
{
  CalibratorExtension::set_nchan( _nchan );

  if (response.size() == _nchan)
    return;

  response.resize( _nchan );
  construct ();
}

//! Set the weight of the specified channel
void Pulsar::PolnCalibratorExtension::set_weight (unsigned ichan, float weight)
{
  CalibratorExtension::set_weight (ichan, weight);
  if (weight == 0)
    set_valid (ichan, false);
}

bool Pulsar::PolnCalibratorExtension::get_valid (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_valid");
  return response[ichan];
}

void Pulsar::PolnCalibratorExtension::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::set_valid");

  if (!valid) {
    response[ichan] = 0;
    weight[ichan] = 0;
  }
  else if (!response[ichan])
    response[ichan] = new_transformation ();
}

unsigned Pulsar::PolnCalibratorExtension::get_nparam () const
{
  if (!nparam)
    const_cast<PolnCalibratorExtension*>(this)->construct();
  return nparam;
}

//! Get the transformation for the specified frequency channel
::MEAL::Complex2* 
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return response[ichan];
}

//! Get the transformation for the specified frequency channel
const ::MEAL::Complex2*
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

  nparam = 0;

  for (unsigned ichan=0; ichan<response.size(); ichan++)
    response[ichan] = new_transformation();

  if (response.size())
    nparam = response[0]->get_nparam();
}


//! Return a new MEAL::Complex2 instance, based on type attribute
MEAL::Complex2* Pulsar::PolnCalibratorExtension::new_transformation ()
{
  switch (type) {
  case Calibrator::SingleAxis:
    return new Calibration::SingleAxis;
  case Calibrator::Polar:
    return new MEAL::Polar;
  case Calibrator::Hamaker:
    return new MEAL::Polar;
  case Calibrator::Britton:
    return new Calibration::Instrument;
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
  case Hybrid:
    return "Hybrid";
  case Corrections:
    return "Corrections";
  }

  return "Invalid";

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

