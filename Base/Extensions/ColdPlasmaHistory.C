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
}

//! Copy constructor
Pulsar::ColdPlasmaHistory::ColdPlasmaHistory (const ColdPlasmaHistory& that)
  : Extension (that)
{
  relative = that.relative;
  absolute = that.absolute;
}

void Pulsar::ColdPlasmaMeasure::set_reference_frequency (double MHz)
{
  reference_wavelength = speed_of_light / (MHz * 1e6);
}

double Pulsar::ColdPlasmaMeasure::get_reference_frequency () const
{
  return 1e-6 * speed_of_light / reference_wavelength;
}
