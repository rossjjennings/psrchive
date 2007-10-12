/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Subtract.h"
#include "Pulsar/Profile.h"

void Pulsar::Subtract::transform (Profile* profile)
{
  profile->operator-=( *get_operand() );
}
