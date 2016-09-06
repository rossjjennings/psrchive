/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

using namespace std;

//! Returns a pointer to a new PolnProfile instance specified by ichan
Pulsar::PolnProfile* Pulsar::Integration::new_PolnProfile (unsigned ichan)
{
  if (Profile::verbose)
    cerr << "Pulsar::Integration::new_PolnProfile" << endl;

  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::new_PolnProfile",
		 "incomplete polarization information");

  return new PolnProfile (get_basis(), get_state(), 
			  get_Profile(0,ichan), get_Profile(1,ichan),
			  get_Profile(2,ichan), get_Profile(3,ichan));
}

const Pulsar::PolnProfile*
Pulsar::Integration::new_PolnProfile (unsigned ichan) const
{
  if (Profile::verbose)
    cerr << "Pulsar::Integration::new_PolnProfile" << endl;

  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::new_PolnProfile",
		 "incomplete polarization information");

  return new PolnProfile (get_basis(), get_state(), 
			  get_Profile(0,ichan)->clone(),
			  get_Profile(1,ichan)->clone(),
			  get_Profile(2,ichan)->clone(),
			  get_Profile(3,ichan)->clone());
}
