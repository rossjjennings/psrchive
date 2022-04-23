/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ParametersLookup.h"
#include "TemporaryDirectory.h"
#include "DirectoryLock.h"
#include "load_factory.h"
#include "tempo++.h"
#include "dirutil.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

using namespace std;

static TemporaryDirectory directory ("psrcat");
static DirectoryLock dir_lock;

Pulsar::Parameters* 
Pulsar::Parameters::Lookup::operator() (const string& name) const try
{
  if (!name.length())
    throw Error (InvalidParam, "Pulsar::Parameters::Lookup::operator()",
		 "empty name");

  string psr_name = name;

  if (name[0] == 'J' || name[0] == 'B')
    psr_name.erase (0,1);

  for (unsigned ipath=0; ipath<path.size(); ipath++)
  {
    for (unsigned iext=0; iext < ext.size(); iext++)
    {
      string filename = path[ipath] + psr_name + ext[iext];

      if (verbose)
	cerr << "Pulsar::Parameters::Lookup '" << filename << "'" << endl;

      if (file_exists(filename.c_str()))
      {
	if (verbose)
	  cerr << "Pulsar::Parameters::Lookup using " << filename << endl;

	return factory<Pulsar::Parameters> (filename);
      }
    }
  }
  
  /* Create name.eph in local directory */ 

  string command = "psrinfo -e " + psr_name;
  string catalogue = "psrinfo";

  if (getenv("PSRCAT_FILE") != 0)
  {
    catalogue = "psrcat";
    command = catalogue;

    if (getenv("PSRCAT_RUNDIR") != 0)
      command += " -all ";

    command += " -e " + psr_name + " > " + psr_name + ".eph";
  }

  if (verbose)
    cerr << "Pulsar::Parameters::Lookup:: Creating ephemeris by " << catalogue 
	 << " -e " << psr_name <<endl;

  dir_lock.set_directory( directory.get_directory() );
  DirectoryLock::Push raii (dir_lock);

  // start with a clean working directory
  dir_lock.clean ();

  int retval = system(command.c_str());

  if (retval != 0)
    throw Error (FailedSys, "Pulsar::Parameters::Lookup::operator()"
	"system (" + command + ")");

  vector<string> filenames;
  dirglob (&filenames, "*.eph");

  if (filenames.size() != 1)
    throw Error (InvalidState, "Pulsar::Parameters::Lookup::operator()",
		 "%s created %d files", catalogue.c_str(), filenames.size());

  string filename = filenames[0];

  if (file_exists(filename.c_str()))
  {
    if (verbose)
      cerr << "Pulsar::Parameters::Lookup using '" + filename + "'" << endl;

    Pulsar::Parameters * params = factory<Pulsar::Parameters> (filename);
    return params;
  }

  throw Error (InvalidParam, "Pulsar::Parameters::Lookup::operator()",
               "Cannot find "+ filename +" after call to "+ catalogue); 
}
catch (Error& error)
{
  throw error += "Pulsar::Parameters::Lookup::operator()";
}

