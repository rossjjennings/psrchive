/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/FrequencyIntegrate.h"

using namespace std;

Pulsar::FrequencyIntegrate operation;
Pulsar::FrequencyIntegrate::EvenlySpaced policy;

int static_init ()
{
  operation.set_range_policy( &policy );
}

static int init = static_init ();


/*!
  \param nscrunch number of neighbouring frequency channels to
  integrate; if zero, then all channels are integrated into one
 */
void Pulsar::Integration::fscrunch (unsigned nscrunch)
{
  policy.set_nintegrate (nscrunch);
  operation.transform (this);
}
