/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Index.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfile.h"

using namespace std;

Pulsar::Index::Index (unsigned value, bool flag)
{
  index = value;
  integrate = flag;
}

const Pulsar::Profile* 
Pulsar::get_Profile (const Archive* data,
		     Index subint, Index pol, Index chan)
{
  if (!data)
    throw Error (InvalidParam, "Pulsar::get_Profile", "no Archive");

  try {

    if (Archive::verbose > 2) cerr << "Pulsar::get_Profile "
      "(" << subint << "," << pol << "," << chan << ")" << endl;

    bool pscrunched = data->get_npol() == 1
      || data->get_state () == Signal::Stokes
      || data->get_state () == Signal::Intensity;

    Reference::To<const Integration> integration;
    integration = get_Integration( data, subint );

    Reference::To<Integration> integration_clone;
    
    if (chan.get_integrate())
    {
      if (Archive::verbose > 2)
	cerr << "Pulsar::get_Profile chan=I fscrunch" << endl;

      if (!integration_clone)
	integration_clone = integration->clone();

      if (pol.get_integrate())
      {
	if (Archive::verbose > 2)
	  cerr << "Pulsar::get_Profile pol=I pscrunch" << endl;
	integration_clone->expert()->pscrunch();
	pscrunched = true;
      }

      integration_clone->expert()->fscrunch();
      integration = integration_clone;
    }

    Reference::To<const Profile> profile;

    if (pol.get_integrate() && !pscrunched)
    {
      if (Archive::verbose > 2)
	cerr << "Pulsar::get_Profile pol=I pscrunch optimization" << endl;

      Reference::To<Profile> temp;
      temp = integration -> get_Profile (0, chan.get_value()) -> clone();
      temp -> sum( integration -> get_Profile (1, chan.get_value()) );

      profile = temp;
    }
    else
      profile = integration -> get_Profile (pol.get_value(), chan.get_value());
    
    // ensure that no one destroys the profile when they go out of scope
    integration = 0;
    integration_clone = 0;

    return profile.release();
  }
  catch (Error& error)
  {
    throw error += "Pulsar::get_Profile (Index)";
  }
}

const Pulsar::Integration* 
Pulsar::get_Integration (const Archive* data, Index subint)
{
  if (!data)
    throw Error (InvalidParam, "Pulsar::get_Integration", "no Archive");

  try
  {
    if (Archive::verbose > 2) cerr << "Pulsar::get_Integration "
      "(" << subint << ")" << endl;

    Reference::To<const Archive> archive = data;

    if (subint.get_integrate())
    {
      if (Archive::verbose > 2)
	cerr << "Pulsar::get_Integration chan=I tscrunch" << endl;

      Reference::To<Archive> archive_clone = archive->clone();
      archive_clone->tscrunch();
      archive = archive_clone;
    }

    Reference::To<const Integration> integration;
    integration = archive->get_Integration(subint.get_value());
    
    // ensure archive doesn't destroy integration when it goes out of scope
    archive = 0;

    return integration.release();
  }
  catch (Error& error)
  {
    throw error += "Pulsar::get_Integration (Index)";
  }
}

//! Return a newly constructed PolnProfile with state == Stokes
const Pulsar::PolnProfile* 
Pulsar::get_Stokes (const Archive* data, Index subint, Index chan)
{
  if (!data)
    throw Error (InvalidParam, "Pulsar::get_Stokes (Index)", "no Archive");

  try
  {
    if (Archive::verbose > 2) cerr << "Pulsar::get_Stokes "
      "(" << subint << "," << chan << ")" << endl;

    Reference::To<const Integration> integration;
    integration = get_Integration( data, subint );

    Reference::To<Integration> integration_clone;
    
    if (chan.get_integrate())
    {
      integration_clone = integration->clone();
      integration_clone->expert()->fscrunch();
      integration = integration_clone;
    }

    Reference::To<const PolnProfile> profile;
    profile = integration->new_PolnProfile(chan.get_value());

    Reference::To<PolnProfile> profile_clone;
    if (integration_clone)
      profile_clone = integration_clone->new_PolnProfile(chan.get_value());

    if (profile->get_state() != Signal::Stokes)
    {
      if (!profile_clone)
	profile_clone = profile->clone();
      profile_clone->convert_state(Signal::Stokes);
      profile = profile_clone;
    }
    
    // ensure profile_clone doesn't destroy profile as it goes out of scope
    profile_clone = 0;
    
    return profile.release();

  }
  catch (Error& error) {
    throw error += "Pulsar::get_Stokes (Index)";
  }
}

std::ostream& Pulsar::operator << (std::ostream& os, const Index& i)
{
  if (i.get_integrate())
    return os << "I";
  else
    return os << i.get_value();
}

std::istream& Pulsar::operator >> (std::istream& is, Index& i)
{
  if (is.peek() == 'I')
  {
    is.get();
    i.set_integrate(true);
  }
  else
  {
    unsigned val;
    is >> val;
    i.set_value(val);
  }

  return is;
}
