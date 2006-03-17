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
  Reference::To<const Archive> archive = data;
  Reference::To<Archive> archive_clone;

  if (subint.get_integrate()) {
    Reference::To<Archive> archive_clone = archive->clone();
    archive_clone->tscrunch();
    archive = archive_clone;
  }

  Reference::To<const Integration> integration;
  integration = archive->get_Integration(subint.get_value());

  Reference::To<Integration> integration_clone;
  if (archive_clone)
    integration_clone = archive_clone->get_Integration(subint.get_value());

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
  archive = 0;
  archive_clone = 0;

  return profile.release();
}


//! Return a newly constructed PolnProfile with state == Stokes
const Pulsar::PolnProfile* 
Pulsar::get_Stokes (const Archive* data, PlotIndex subint, PlotIndex chan)
{
  Reference::To<const Archive> archive = data;
  Reference::To<Archive> archive_clone;

  if (subint.get_integrate()) {
    Reference::To<Archive> archive_clone = archive->clone();
    archive_clone->tscrunch();
    archive = archive_clone;
  }

  Reference::To<const Integration> integration;
  integration = archive->get_Integration(subint.get_value());

  Reference::To<Integration> integration_clone;
  if (archive_clone)
    integration_clone = archive_clone->get_Integration(subint.get_value());

  if (chan.get_integrate()) {
    if (!integration_clone)
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
