/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/HasArchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

// #define _DEBUG 1

Pulsar::HasArchive::HasArchive ()
{
}

Pulsar::HasArchive::~HasArchive ()
{
}

void Pulsar::HasArchive::set_Archive (const Archive* est)
{
#if _DEBUG
    cerr << "Pulsar::HasArchive::set_Archive this=" << this
	 << " est=" << est << endl;
#endif

  archive = est;

  integration = 0;
  profile = 0;

#if _DEBUG
    cerr << "Pulsar::HasArchive::set_Archive this=" << this << " done" << endl;
#endif
}

const Pulsar::Archive* Pulsar::HasArchive::get_Archive () const
{
  return archive;
}

const Pulsar::Integration* Pulsar::HasArchive::get_Integration () const
{
  if (!integration)
    integration = Pulsar::get_Integration (archive, isubint);
  return integration;
}

const Pulsar::Profile* Pulsar::HasArchive::get_Profile () const
{
  if (!profile)
    profile = Pulsar::get_Profile (get_Integration(), ipol, ichan);
  return profile;
}

//! Set the sub-integration from which statistics will be drawn
void Pulsar::HasArchive::set_subint (Index _isubint)
{
#if _DEBUG
    cerr << "Pulsar::HasArchive::set_subint this=" << this
         << " index=" << _isubint << endl;
#endif

  isubint = _isubint;
  integration = 0;
  profile = 0;

#if _DEBUG
    cerr << "Pulsar::HasArchive::set_subint this=" << this << " done" << endl;
#endif
}

Pulsar::Index Pulsar::HasArchive::get_subint () const
{
  return isubint;
}

//! Set the frequency channel from which statistics will be drawn
void Pulsar::HasArchive::set_chan (Index _ichan)
{
#if _DEBUG
    cerr << "Pulsar::HasArchive::set_chan this=" << this
         << " index=" << _ichan << endl;
#endif

  ichan = _ichan;
  profile = 0;

#if _DEBUG
    cerr << "Pulsar::HasArchive::set_chan this=" << this << " done" << endl;
#endif
}

Pulsar::Index Pulsar::HasArchive::get_chan () const
{
  return ichan;
}

//! Set the polarization to plot
void Pulsar::HasArchive::set_pol (Index _ipol)
{
#if _DEBUG
    cerr << "Pulsar::HasArchive::set_pol this=" << this
         << " index=" << _ipol << endl;
#endif

  ipol = _ipol;
  profile = 0;

#if _DEBUG
    cerr << "Pulsar::HasArchive::set_pol this=" << this << " done" << endl;
#endif
}

Pulsar::Index Pulsar::HasArchive::get_pol () const
{
  return ipol;
}

bool Pulsar::HasArchive::is_current () const
{
  return integration != 0 && profile != 0;
}

