/***************************************************************************
 *
 *   Copyright (C) 2006-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DispersionHistory.h"

//! Default constructor
Pulsar::DispersionHistory::DispersionHistory ()
  : ColdPlasmaHistory ("DispersionHistory")
{
  set_measurement (ColdPlasmaMeasure::DispersionMeasure);
}

//! Copy constructor
Pulsar::DispersionHistory::DispersionHistory (const DispersionHistory& extension)
  : ColdPlasmaHistory ("DispersionHistory")
{
  ColdPlasmaHistory::operator= (extension);
}

//! Operator =
const Pulsar::DispersionHistory&
Pulsar::DispersionHistory::operator= (const DispersionHistory& extension)
{
  ColdPlasmaHistory::operator= (extension);
  return *this;
}
