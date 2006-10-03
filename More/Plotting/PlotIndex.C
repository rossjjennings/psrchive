/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotIndex.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfile.h"

Pulsar::PlotIndex::PlotIndex (unsigned value, bool flag)
{
  index = value;
  integrate = flag;
}

const Pulsar::Profile* 
Pulsar::get_Profile (const Archive* data,
		     PlotIndex subint, PlotIndex pol, PlotIndex chan)
{
  if (!data)
    throw Error (InvalidParam, "Pulsar::get_Profile", "no Archive");

  Reference::To<const Integration> integration;
  integration = get_Integration( data, subint );

  Reference::To<Integration> integration_clone;

  if (pol.get_integrate()) {
    if (!integration_clone)
      integration_clone = integration->clone();
    integration_clone->expert()->pscrunch();
    integration = integration_clone;
  }

  if (chan.get_integrate()) {
    if (!integration_clone)
      integration_clone = integration->clone();
    integration_clone->expert()->fscrunch();
    integration = integration_clone;
  }

  Reference::To<const Profile> profile;
  profile = integration -> get_Profile (pol.get_value(), chan.get_value());

  // ensure that no one destroys the profile when they go out of scope
  integration = 0;
  integration_clone = 0;

  return profile.release();
}

const Pulsar::Integration* 
Pulsar::get_Integration (const Archive* data, PlotIndex subint)
{
  if (!data)
    throw Error (InvalidParam, "Pulsar::get_Integration", "no Archive");

  Reference::To<const Archive> archive = data;

  if (subint.get_integrate()) {
    Reference::To<Archive> archive_clone = archive->clone();
    archive_clone->tscrunch();
    archive = archive_clone;
  }

  Reference::To<const Integration> integration;
  integration = archive->get_Integration(subint.get_value());

  // ensure that no one destroys the profile when they go out of scope
  archive = 0;

  return integration.release();
}

//! Return a newly constructed PolnProfile with state == Stokes
const Pulsar::PolnProfile* 
Pulsar::get_Stokes (const Archive* data, PlotIndex subint, PlotIndex chan)
{
  Reference::To<const Integration> integration;
  integration = get_Integration( data, subint );

  Reference::To<Integration> integration_clone;

  if (chan.get_integrate()) {
    integration_clone = integration->clone();
    integration_clone->expert()->fscrunch();
    integration = integration_clone;
  }

  Reference::To<const PolnProfile> profile;
  profile = integration->new_PolnProfile(chan.get_value());

  Reference::To<PolnProfile> profile_clone;
  if (integration_clone)
    profile_clone = integration_clone->new_PolnProfile(chan.get_value());

  if (profile->get_state() != Signal::Stokes) {
    if (!profile_clone)
      profile_clone = profile->clone();
    profile_clone->convert_state(Signal::Stokes);
    profile = profile_clone;
  }

  // ensure that profile_clone doesn't destroy instance as it goes out of scope
  profile_clone = 0;

  return profile.release();
}

std::ostream& Pulsar::operator << (std::ostream& os, const PlotIndex& i)
{
  if (i.get_integrate())
    return os << "I";
  else
    return os << i.get_value();
}

std::istream& Pulsar::operator >> (std::istream& is, PlotIndex& i)
{
  if (is.peek() == 'I') {
    is.get();
    i.set_integrate(true);
  }
  else {
    unsigned val;
    is >> val;
    i.set_value(val);
  }

  return is;
}
