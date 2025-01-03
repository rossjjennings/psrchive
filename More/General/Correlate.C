/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Correlate.h"
#include "Pulsar/Profile.h"

using namespace std;

void Pulsar::Correlate::transform (Profile* profile)
{
  if (normalize)
  {
    if (Profile::verbose)
      cerr << "Pulsar::Correlate::transform normalized cross-correlation" << endl;
    profile->correlate_normalized( get_operand() );
  }
  else
    profile->correlate( get_operand() );
}
