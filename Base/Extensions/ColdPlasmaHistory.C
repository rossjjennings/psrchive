/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ColdPlasmaHistory.h"
#include "Physical.h"

//! Default constructor
Pulsar::ColdPlasmaHistory::ColdPlasmaHistory (const char* name)
  : Extension (name)
{
  relative.set_reference(ColdPlasmaMeasure::Relative);
  absolute.set_reference(ColdPlasmaMeasure::Absolute);
}

//! Copy constructor
Pulsar::ColdPlasmaHistory::ColdPlasmaHistory (const ColdPlasmaHistory& that)
  : Extension (that)
{
  relative = that.relative;
  absolute = that.absolute;
}

void Pulsar::ColdPlasmaHistory::set_measurement (ColdPlasmaMeasure::Measurement measurement)
{
  relative.set_measurement (measurement);
  absolute.set_measurement (measurement);
}

void Pulsar::ColdPlasmaMeasure::set_reference_frequency (double MHz)
{
  reference_wavelength = speed_of_light / (MHz * 1e6);
}

double Pulsar::ColdPlasmaMeasure::get_reference_frequency () const
{
  return 1e-6 * speed_of_light / reference_wavelength;
}

void Pulsar::ColdPlasmaMeasure::set_corrected (bool flag)
{
  if (Integration::verbose)
  {
    std::string ref = "Unknown";
    if (reference == Relative)
      ref = "Relative";
    else if (reference == Absolute)
      ref = "Absolute";

    std::string meas = "Unknown";
    if (measurement == DispersionMeasure)
      meas = "DM";
    else if (measurement == RotationMeasure)
      meas = "RM";

    std::cerr << "Pulsar::ColdPlasmaMeasure::set_corrected " << ref << " " << meas << " to " << flag << std::endl; 
  }

  corrected = flag;
}
