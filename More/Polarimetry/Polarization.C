/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Polarization.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

//! Return a newly constructed PolnProfile with state == Stokes
const Pulsar::PolnProfile* 
Pulsar::new_Stokes (const Archive* data, unsigned isubint, unsigned ichan)
{
  return new_Stokes (data->get_Integration (isubint), ichan);
}

const Pulsar::PolnProfile* 
Pulsar::new_Stokes (const Integration* subint, unsigned ichan)
{
  Reference::To<const PolnProfile> profile = subint->new_PolnProfile(ichan);

  if (profile->get_state() != Signal::Stokes) {
    Reference::To<PolnProfile> clone = profile->clone();
    clone->convert_state(Signal::Stokes);
    profile = clone;
  }

  return profile.release();
}

const Pulsar::Profile*
Pulsar::new_Profile (const PolnProfile* data, char code)
{
  Reference::To<Profile> profile;

  std::vector< Estimate<double> > theta;

  if (code == 'O' || code == 'o')
  {
    data->get_orientation ( theta, 0.0 );
    profile = data->get_Profile(2)->clone();
  }

  switch (code) {
  case 'I':
    return data->get_Profile(0);
  case 'Q':
    return data->get_Profile(1);
  case 'U':
    return data->get_Profile(2);
  case 'V':
    return data->get_Profile(3);

  case 'L': {
    // total linearly polarized flux
    profile = data->get_Profile(0)->clone();
    data->get_linear(profile);
    return profile.release();
  }

  case 'p': {
    // total polarized flux
    profile = data->get_Profile(0)->clone();
    data->get_polarized(profile);
    return profile.release();
  }

  case 'S': {
    // polarimetric invariant flux
    profile = data->get_Profile(0)->clone();
    data->invint(profile);
    return profile.release();
  }

  case 'O':
    for (unsigned i=0; i<profile->get_nbin(); i++)
      profile->get_amps()[i] = theta.at(i).val;
    return profile.release();

  case 'o':
    for (unsigned i=0; i<profile->get_nbin(); i++)
      profile->get_amps()[i] = sqrt(theta.at(i).var);
    return profile.release();



  default:
    throw Error (InvalidParam, "Pulsar::get_Profile",
		 "Unknown polarization code '%c'", code);
  }

}

