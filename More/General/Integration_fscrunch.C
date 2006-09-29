/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/FrequencyIntegrate.h"

/*!
  \param nscrunch number of neighbouring frequency channels to
  integrate; if zero, then all channels are integrated into one
 */
void Pulsar::Integration::fscrunch (unsigned nscrunch)
{
  FrequencyIntegrate fscr;

  fscr.set_nscrunch (nscrunch);
  fscr.transform (this);
}
