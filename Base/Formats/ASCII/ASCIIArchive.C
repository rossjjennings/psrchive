/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ASCIIArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include <fstream>

void Pulsar::ASCIIArchive::init ()
{
  integration_length = 0.0;
}

Pulsar::ASCIIArchive::ASCIIArchive()
{
  init ();
}

Pulsar::ASCIIArchive::~ASCIIArchive()
{
  // destroy any ASCIIArchive resources
}

Pulsar::ASCIIArchive::ASCIIArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASCIIArchive::ASCIIArchive (const ASCIIArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive construct copy ASCIIArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::ASCIIArchive::ASCIIArchive (const Archive& arch, 
				    const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::ASCIIArchive::copy (const Archive& archive, 
				 const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy dynamic cast call" << endl;

  const ASCIIArchive* like_me = dynamic_cast<const ASCIIArchive*>(&archive);
  if (!like_me)
    return;

  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy another ASCIIArchive" << endl;

  integration_length = like_me -> integration_length;
}

Pulsar::ASCIIArchive* Pulsar::ASCIIArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::clone" << endl;
  return new ASCIIArchive (*this);
}

Pulsar::ASCIIArchive* 
Pulsar::ASCIIArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::extract" << endl;
  return new ASCIIArchive (*this, subints);
}


void Pulsar::ASCIIArchive::load_header (const char* filename)
{
  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::ASCIIArchive::load_header",
		 "ifstream(%s)", filename);

  is >> centre_frequency >> nbin >> source >> integration_length;

  if (is.fail())
    throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_header",
		 "Could not parse header from%s", filename);

  set_nchan   (1);
  set_npol    (4);
  set_nsubint (1);
  set_state (Signal::Stokes);
}


Pulsar::Integration*
Pulsar::ASCIIArchive::load_Integration (const char* filename, unsigned subint)
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::load_Integration " << filename << " " 
	 << subint << endl;

  // load all BasicIntegration attributes and data from filename

  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::ASCIIArchive::load_Integration",
		 "ifstream(%s)", filename);

  // get the header line
  string line;
  getline (is, line);

  Pulsar::BasicIntegration* integration = new BasicIntegration;
  resize_Integration (integration);
  init_Integration (integration);

  float* I = integration->get_Profile (0, 0) -> get_amps();
  float* Q = integration->get_Profile (1, 0) -> get_amps();
  float* U = integration->get_Profile (2, 0) -> get_amps();
  float* V = integration->get_Profile (3, 0) -> get_amps();

  unsigned bin = 0;
  float unused;

  integration->set_folding_period (1.0);

  for (unsigned ibin=0; ibin < get_nbin(); ibin++) {

    is >> bin >> I[ibin] >> Q[ibin] >> U[ibin] >> V[ibin];

    for (unsigned i=0; i<4; i++)
      is >> unused;

    if (is.fail() || bin != ibin)
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "Could not parse data from %s", filename);

  }

  return integration;
}

void Pulsar::ASCIIArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and ASCIIArchive attributes as well as
  // BasicIntegration attributes and data to filename

  throw Error (InvalidState, "Pulsar::ASCIIArchive::unload_file",
	       "not implemented");
}



string Pulsar::ASCIIArchive::Agent::get_description () 
{
  return "ASCII Archive Version 1.0";
}


bool Pulsar::ASCIIArchive::Agent::advocate (const char* filename)
  try {
    
    ASCIIArchive archive;
    archive.load_header (filename);
    return true;

  }
  catch (Error& e) {
    return false;
  }

