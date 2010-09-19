/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AuxColdPlasma.h"

//! Default constructor
Pulsar::AuxColdPlasma::AuxColdPlasma ()
  : Extension ("AuxColdPlasma")
{
  dispersion_corrected = birefringence_corrected = false;
}
