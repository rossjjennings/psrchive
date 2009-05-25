/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ASCIIArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include <fstream>
using namespace std;

void Pulsar::ASCIIArchive::init ()
{
  integration_length = 0.0;
  period = 1.0;
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

void Pulsar::ASCIIArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::ASCIIArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

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

void Pulsar::ASCIIArchive::load_header (const char* filename)
{
  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::ASCIIArchive::load_header",
		 "ifstream(%s)", filename);

  // clear the optional hash
  if (is.peek() == '#')
    hashed_header (is);
  else
    is >> centre_frequency >> nbin >> source >> integration_length;

  if (is.fail())
    throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_header",
		 "could not parse header from '%s'", filename);

  // get the remainder of the header line, if any
  string line;
  getline (is, line);

  // get the first line of data
  getline (is, line);

  double v;
  if (sscanf (line.c_str(), "%lf %lf %lf %lf %lf", &v,&v,&v,&v,&v) == 5)
  {
    set_npol (4);
    set_state (Signal::Stokes);
  }
  else
  {
    set_npol (1);
    set_state (Signal::Intensity);
  }

  set_nchan   (1);
  set_nsubint (1);

}

void Pulsar::ASCIIArchive::hashed_header (istream& is)
{
  // clear the hash
  is.get();

  double epoch_MJD = 0;
  is >> epoch_MJD;

  double epoch_seconds = 0;
  is >> epoch_seconds;

  epoch = MJD(epoch_MJD);
  epoch += epoch_seconds;

  is >> period;
    
  int subints = 0;
  is >> subints;

  is >> centre_frequency >> dispersion_measure >> nbin;

  char site = 0;
  is >> site;

  int ignore = 0;
  is >> ignore;

  is >> source;

  double phase;
  is >> phase;
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

  unsigned bin = 0;

  integration->set_folding_period (period);
  integration->set_epoch (epoch);

  if (Profile::no_amps)
    return integration;

  for (unsigned ibin=0; ibin < get_nbin(); ibin++)
  {
    if (verbose > 2)
      cerr << "Pulsar::ASCIIArchive::load_Integration ibin=" << ibin << endl;

    is >> bin;

    for (unsigned ipol=0; ipol < get_npol(); ipol++)
    {
      double value;
      is >> value;

      integration->get_Profile(ipol, 0) -> get_amps()[ibin] = value;
    }

    if (is.fail())
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "data read failed");

    if (bin != ibin)
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "bin number=%u != expected=%u", bin, ibin);

    // clear the remainder of the line
    getline (is, line);

    if (is.fail())
      throw Error (InvalidParam, "Pulsar::ASCIIArchive::load_Integration",
		   "clear read failed");
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


bool Pulsar::ASCIIArchive::Agent::advocate (const char* filename) try
{
  ASCIIArchive archive;
  archive.load_header (filename);
  return true;
}
catch (Error& e) 
{
  if (Archive::verbose > 2)
    cerr << e << endl;

  return false;
}
catch (std::exception& e)
{
  return false;
}
