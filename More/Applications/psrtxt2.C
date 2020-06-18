/***************************************************************************
 *
 *   Copyright (C) 2008 - 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/MoreProfiles.h"
// #include "TextInterfaceAttribute.h"

using namespace std;
using namespace Pulsar;

//
//! An example of an application
//
class example : public Pulsar::Application
{
public:

  //! Default constructor
  example ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  string subint_indeces;
  string chan_indeces;
  string bin_indeces;
  string pol_indeces;

  //! Add command line options
  void add_options (CommandLine::Menu&);
};


/*!

  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "example -h"

  This constructor makes use of

  - StandardOptions (-j -J etc.): an option set that provides standard
  preprocessing with the pulsar command language interpreter.

  - UnloadOptions (-e -m etc.): an option set that provides standard
  options for unloading data.

  This constructor also sets the default values of the attributes that
  are unique to the program.

*/

example::example ()
  : Application ("example", "example psrchive program")
{
  add( new Pulsar::StandardOptions );
}


/*!

  Add application-specific command-line options.

*/

void example::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (subint_indeces, 'i', "subints");
  arg->set_help ("select the sub-integration(s) to output");

  arg = menu.add (chan_indeces, 'c', "chans");
  arg->set_help ("select the frequency channel(s) to output");

  arg = menu.add (bin_indeces, 'b', "bins");
  arg->set_help ("select the phase bin(s) to output");

  arg = menu.add (pol_indeces, 's', "pols");
  arg->set_help ("select the polarization(s) to output");
}


/*!

  Scale every profile and optionally set the source name

*/

vector<unsigned> indeces (unsigned n, const string& txt)
{
  vector<unsigned> result;
  if (txt == "")
    {
      result.resize (n);
      for (unsigned i=0; i<n; i++) result[i] = i;
    }
  else
    TextInterface::parse_indeces (result, "[" + txt + "]", n);

  return result;
}

class polarization
{
  string indeces;
  bool auxiliary;
  vector< Reference::To<const Profile> > profiles;

  Reference::To<Integration> subint;

public:
  polarization (string txt) { indeces = txt; auxiliary = false; }

  void setup (Integration* subint, unsigned ichan)
  {
    Pulsar::MoreProfiles* more = NULL;

    unsigned npol = subint->get_npol();
    if (auxiliary)
    {
      more = subint->get_Profile (0,ichan)->get<Pulsar::MoreProfiles>();
      npol = more->get_size();
    }

    try {
      vector<unsigned> pols = ::indeces (npol, indeces);
      profiles.resize (pols.size());

      for (unsigned ipol=0; ipol < pols.size(); ipol++)
      {
	if (more)
	  profiles[ipol] = more->get_Profile( pols[ipol] );
	else
	  profiles[ipol] = subint->get_Profile ( pols[ipol],ichan);
      }
    } 
    catch (...)
    {
      // if parsing indeces fails, then assume that letter codes are used
      Reference::To<const PolnProfile> profile = new_Stokes (subint, ichan);
      profiles.resize (indeces.length());
      for (unsigned ipol=0; ipol < indeces.length(); ipol++)
	profiles[ipol] = new_Profile (profile, indeces[ipol]);
    } 
  }

  unsigned size () { return profiles.size(); }
  const Profile* get_Profile (unsigned ipol) { return profiles.at(ipol); }   
};

void example::process (Pulsar::Archive* archive)
{
  archive->remove_baseline();

  vector<unsigned> subints = indeces (archive->get_nsubint(), subint_indeces);
  vector<unsigned> chans = indeces (archive->get_nchan(), chan_indeces);
  vector<unsigned> bins = indeces (archive->get_nbin(), bin_indeces);

#if _DEBUG
  cerr << "subints=" << subints.size() << endl;
  cerr << "chans=" << chans.size() << endl;
  cerr << "bins=" << bins.size() << endl;
#endif

  polarization pols (pol_indeces);

  for (unsigned isub=0; isub < subints.size(); isub++)
  {
    Pulsar::Integration* subint = archive->get_Integration (subints[isub]);

    for (unsigned ichan=0; ichan < chans.size(); ichan++)
    {
      pols.setup (subint, chans[ichan]);

      for (unsigned ibin=0; ibin < bins.size(); ibin++)
      {
	cout << subints[isub] << " " << chans[ichan] << " " << bins[ibin];

	for (unsigned ipol=0; ipol < pols.size(); ipol++)
	  cout << " " << pols.get_Profile(ipol)->get_amps()[ bins[ibin] ];

	cout << endl;
      }
    }
  }
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  example program;
  return program.main (argc, argv);
}

