#include "Pulsar/ExampleArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

void Pulsar::ExampleArchive::init ()
{
  // initialize the ExampleArchive attributes
}

Pulsar::ExampleArchive::ExampleArchive()
{
  init ();
}

Pulsar::ExampleArchive::~ExampleArchive()
{
  // destroy any ExampleArchive resources
}

Pulsar::ExampleArchive::ExampleArchive (const Archive& arch)
{
  if (verbose)
    cerr << "ExampleArchive construct copy Archive" << endl;

  init ();
  ExampleArchive::copy (arch);
}

Pulsar::ExampleArchive::ExampleArchive (const ExampleArchive& arch)
{
  if (verbose)
    cerr << "ExampleArchive construct copy ExampleArchive" << endl;

  init ();
  ExampleArchive::copy (arch);
}

Pulsar::ExampleArchive::ExampleArchive (const Archive& arch, 
				  const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "ExampleArchive construct extract Archive" << endl;

  init ();
  ExampleArchive::copy (arch, subints);
}

Pulsar::ExampleArchive::ExampleArchive (const ExampleArchive& arch, 
				  const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "ExampleArchive construct extract ExampleArchive" << endl;

  init ();
  ExampleArchive::copy (arch, subints);
}

void Pulsar::ExampleArchive::copy (const Archive& archive, 
				const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "ExampleArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose)
    cerr << "ExampleArchive::copy dynamic cast call" << endl;
  
  const ExampleArchive* farchive = dynamic_cast<const ExampleArchive*>(&archive);
  if (!farchive)
    return;
  
  if (verbose)
    cerr << "ExampleArchive::copy another ExampleArchive" << endl;

  // copy ExampleArchive attributes
}

Pulsar::Archive* Pulsar::ExampleArchive::clone () const
{
  if (verbose)
    cerr << "ExampleArchive::clone" << endl;
  return new ExampleArchive (*this);
}

Pulsar::Archive* 
Pulsar::ExampleArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose)
    cerr << "ExampleArchive::extract" << endl;
  return new ExampleArchive (*this, subints);
}



void Pulsar::ExampleArchive::load_header (const char* filename)
{
  // load all BasicArchive and ExampleArchive attributes from filename
}

Pulsar::Integration*
Pulsar::ExampleArchive::load_Integration (const char* filename, unsigned subint)
{
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // load all BasicIntegration attributes and data from filename

  return integration;
}

void Pulsar::ExampleArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ExampleArchive attributes as well as
  // BasicIntegration attributes and data to filename
}



string Pulsar::ExampleArchive::Agent::get_description () 
{
  return "Example Archive Version 1.0";
}

bool Pulsar::ExampleArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by Example Archive, this method should return true

  return false;
}

