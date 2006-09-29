/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

/*! 
  \pre The profile baselines must have been removed (unchecked).
*/
void Pulsar::Integration::invint ()
{
  Reference::To<PolnProfile> profile;

  for (unsigned ichan=0; ichan<get_nchan(); ++ichan) {

    profile = new_PolnProfile (ichan);

    profile -> invint ( profile->get_Profile(0) );

  } // for each channel

  resize (1);

}
