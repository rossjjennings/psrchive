/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AuxColdPlasmaHistory.h"

//! Default constructor
Pulsar::AuxColdPlasmaHistory::AuxColdPlasmaHistory (const char* name)
  : Extension (name)
{
  measure = 0.0;
  corrected = false;
}

//! Set the dispersion/rotation measure
void Pulsar::AuxColdPlasmaHistory::set_measure (double m)
{
  measure = m;
}

//! Get the dispersion/rotation measure
double Pulsar::AuxColdPlasmaHistory::get_measure () const
{
  return measure;
}

//! Set the corrected flag
void Pulsar::AuxColdPlasmaHistory::set_corrected (bool flag)
{
  corrected = flag;
}

//! Get the corrected flag
bool Pulsar::AuxColdPlasmaHistory::get_corrected () const
{
  return corrected;
}

