#include "Pulsar/EPNArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

void Pulsar::EPNArchive::init ()
{
  // initialize the EPNArchive attributes
}

Pulsar::EPNArchive::EPNArchive()
{
  init ();
}

Pulsar::EPNArchive::~EPNArchive()
{
  // destroy any EPNArchive resources
}

Pulsar::EPNArchive::EPNArchive (const Archive& arch)
{
  if (verbose)
    cerr << "EPNArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPNArchive::EPNArchive (const EPNArchive& arch)
{
  if (verbose)
    cerr << "EPNArchive construct copy EPNArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPNArchive::EPNArchive (const Archive& arch, 
				const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "EPNArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::EPNArchive::copy (const Archive& archive, 
			       const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "EPNArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose)
    cerr << "EPNArchive::copy dynamic cast call" << endl;
  
  const EPNArchive* like_me = dynamic_cast<const EPNArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose)
    cerr << "EPNArchive::copy another EPNArchive" << endl;

  // copy EPNArchive attributes
}

Pulsar::Archive* Pulsar::EPNArchive::clone () const
{
  if (verbose)
    cerr << "EPNArchive::clone" << endl;
  return new EPNArchive (*this);
}

Pulsar::Archive* 
Pulsar::EPNArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose)
    cerr << "EPNArchive::extract" << endl;
  return new EPNArchive (*this, subints);
}



void Pulsar::EPNArchive::load_header (const char* filename)
{
  // load all BasicArchive and EPNArchive attributes from filename
}

Pulsar::Integration*
Pulsar::EPNArchive::load_Integration (const char* filename, unsigned subint)
{
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // load all BasicIntegration attributes and data from filename

  return integration;
}

void Pulsar::EPNArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and EPNArchive attributes as well as
  // BasicIntegration attributes and data to filename
}



string Pulsar::EPNArchive::Agent::get_description () 
{
  return "EPN Archive Version 1.0";
}

bool Pulsar::EPNArchive::Agent::advocate (const char* filename)
{

  return false;
}

