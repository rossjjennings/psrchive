/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Correlate.h"
#include "Pulsar/Profile.h"

void Pulsar::Correlate::transform (Profile* profile)
{
  profile->correlate( get_operand() );
}
