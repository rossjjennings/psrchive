/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BirefringenceHistory.h"

//! Default constructor
Pulsar::BirefringenceHistory::BirefringenceHistory ()
  : ColdPlasmaHistory ("BirefringenceHistory")
{
  set_measurement (ColdPlasmaMeasure::RotationMeasure);
}

//! Copy constructor
Pulsar::BirefringenceHistory::BirefringenceHistory (const BirefringenceHistory& extension)
  : ColdPlasmaHistory ("BirefringenceHistory")
{
  ColdPlasmaHistory::operator= (extension);
}

//! Operator =
const Pulsar::BirefringenceHistory&
Pulsar::BirefringenceHistory::operator= (const BirefringenceHistory& extension)
{
  ColdPlasmaHistory::operator= (extension);
  return *this;
}
