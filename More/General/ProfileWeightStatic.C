/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileWeightStatic.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"
#include "Ranges.h"

using namespace std;

Pulsar::ProfileWeightStatic* Pulsar::ProfileWeightStatic::clone () const
{
  return new ProfileWeightStatic (*this);
}

//! Set the Profile from which the PhaseWeight will be derived
void Pulsar::ProfileWeightStatic::set_Profile (const Profile* _profile)
{
  profile = _profile;
  range.set_nbin( profile->get_nbin() );
}

void Pulsar::ProfileWeightStatic::calculate (PhaseWeight* weight)
{
  unsigned nbin = profile->get_nbin();
  weight->resize (nbin);
  weight->set_all (0.0);

  range.set_nbin( nbin );
  Phase::Ranges bins = range.as( Phase::Bins );

  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (bins.within (ibin))
      (*weight)[ ibin ] = 1.0;
}

class Pulsar::ProfileWeightStatic::Interface 
  : public TextInterface::To<ProfileWeightStatic>
{
public:
  Interface (ProfileWeightStatic* instance)
  {
    if (instance)
      set_instance (instance);

    add( &ProfileWeightStatic::get_range,
	 &ProfileWeightStatic::set_range,
	 "range", "phase range(s) in window" );
  }

  std::string get_interface_name () const { return "set"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ProfileWeightStatic::get_interface ()
{
  return new Interface (this);
}

