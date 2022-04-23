/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ParametersLookup.h"
#include "TemporaryDirectory.h"
#include "DirectoryLock.h"
#include "load_factory.h"
#include "tempo++.h"
#include "dirutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

Pulsar::Parameters::Lookup::Lookup () 
{
  ext.push_back (".eph");
  ext.push_back (".par");

  try
  {
    string tempo_pardir = Tempo::get_configuration("PARDIR");
    if (tempo_pardir.length())
      path.push_back (tempo_pardir);
  }
  catch (Error& error)
  {
    if (verbose)
      cerr << "Pulsar::Parameters::Lookup error " << error.get_message() << endl;
  }
}

void Pulsar::Parameters::Lookup::add_extension (const string& e)
{
  if (!e.length())
    return;

  string edot = e;

  if (e[0] != '.')
    edot = "." + e;

  ext.push_back (edot);
}    

void Pulsar::Parameters::Lookup::add_path (const string& p)
{
  path.push_back (p);
}

string Pulsar::Parameters::Lookup::get_param (const string& param, const string name) const try
{
  if (!name.length())
    throw Error (InvalidParam, "Pulsar::Parameters::Lookup::get_param",
     "empty name");

  string psr_name = name;
  if (name[0] == 'J' || name[0] == 'B')
    psr_name.erase (0,1);

  string command;

  if (getenv("PSRCAT_FILE") != 0)
  {
    command = "psrcat";

    if (getenv("PSRCAT_RUNDIR") != 0)
      command += " -all ";

    command += " -c " + param + " " + psr_name + " -nohead -nonumber";
  }
  else
  {
    if (verbose)
      cerr << "Pulsar::Parameters::Lookup::get_param psrcat not available at compilation time" << endl;
    return string("*");
  }

  char buffer[128];
  std::string result = "";
  FILE* pipe = popen (command.c_str(), "r");
  if (!pipe)
    throw Error (FailedSys, "Pulsar::Parameters::Lookup::get_param",
                 "failed to open command %s", command.c_str());
  try
  {
    while (fgets(buffer, sizeof buffer, pipe) != NULL)
    {
      result += buffer;
    }
  }
  catch (...)
  {
    pclose(pipe);
    throw Error (FailedSys, "Pulsar::Parameters::Lookup::get_param",
                 "failed to read command output %s", command.c_str());
  }
  pclose(pipe);

  // trim whitespace/newline
  const std::string WHITESPACE = " \n\r\t\f\v";
  size_t start = result.find_first_of(WHITESPACE);
  if (start != std::string::npos)
    result.resize(start);

  // check if the parameter was defined
  string not_found = "not known.  Use psrcat -p to get a list of parameters";
  if (result.find(not_found) != string::npos)
    throw Error (FailedSys, "Pulsar::Parameters::Lookup::get_param",
                 "parameter not found in catalogue");

  // check if the pulsar was in the catalogue
  string not_in_catalgoue = "not in catalogue";
  if (result.find(not_found) != string::npos)
    throw Error (FailedSys, "Pulsar::Parameters::Lookup::get_param",
                 "Pulsar not found in catalogue");

  return result;
}
catch (Error& error)
{
  throw error += "Pulsar::Parameters::Lookup::get_param";
}

