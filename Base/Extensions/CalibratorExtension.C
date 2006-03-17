/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/Calibrator.h"


using namespace Pulsar;

//! Default constructor
CalibratorExtension::CalibratorExtension (const char* name)
  : Extension (name)
{
}

//! Copy constructor
CalibratorExtension::CalibratorExtension (const CalibratorExtension& copy)
  : Extension (copy.get_extension_name().c_str())
{
  operator = (copy);
}

//! Operator =
const CalibratorExtension&
CalibratorExtension::operator= (const CalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  epoch = copy.epoch;
  weight = copy.weight;
  centre_frequency = copy.centre_frequency;

  return *this;
}

//! Destructor
CalibratorExtension::~CalibratorExtension ()
{
}

void Pulsar::CalibratorExtension::set_type (Calibrator::Type _type)
{
  type = _type;
}

Pulsar::Calibrator::Type Pulsar::CalibratorExtension::get_type () const
{
  return type;
}

MJD CalibratorExtension::get_epoch () const
{
  return epoch;
}

void CalibratorExtension::set_epoch (const MJD& _epoch)
{
  epoch = _epoch;
}


//! Set the number of frequency channels
void CalibratorExtension::set_nchan (unsigned nchan)
{
  weight.resize( nchan );
  centre_frequency.resize( nchan );
}

//! Get the number of frequency channels
unsigned CalibratorExtension::get_nchan () const
{
  return weight.size();
}

float CalibratorExtension::get_weight (unsigned ichan) const
{
  range_check (ichan, "Pulsar::CalibratorExtension::get_weight");
  return weight[ichan];
}

void CalibratorExtension::set_weight (unsigned ichan, float _weight)
{
  range_check (ichan, "Pulsar::CalibratorExtension::set_weight");
  weight[ichan] = _weight;
}


double CalibratorExtension::get_centre_frequency (unsigned ichan) const
{
  range_check (ichan, "Pulsar::CalibratorExtension::get_centre_frequency");
  return centre_frequency[ichan];
}

void CalibratorExtension::set_centre_frequency (unsigned ichan, double freq)
{
  range_check (ichan, "Pulsar::CalibratorExtension::set_centre_frequency");
  centre_frequency[ichan] = freq;
}

void CalibratorExtension::range_check (unsigned ichan,
				       const char* method) const
{
  if (ichan >= weight.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
		 ichan, weight.size());
}
