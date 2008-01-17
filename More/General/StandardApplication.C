/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/StandardApplication.h"
#include "Pulsar/Interpreter.h"

#include "strutil.h"

Pulsar::StandardApplication::StandardApplication (const std::string& name,
						  const std::string& desc)
  : Application (name, desc)
{
}

//! Extra usage information implemented by derived classes
std::string Pulsar::StandardApplication::get_usage ()
{
  return 
    " -j commands      execute pulsar shell preprocessing commands \n"
    " -J script        execute pulsar shell preprocessing script \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::StandardApplication::get_options ()
{
  return "j:J:";
}

//! Parse a non-standard command
bool Pulsar::StandardApplication::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'j':
    separate (optarg, jobs, ",");
    break;
    
  case 'J':
    loadlines (optarg, jobs);
    break;

  default:
    return false;
  }

  return true;
}

//! Preprocessing tasks implemented by partially derived classes
void Pulsar::StandardApplication::preprocess (Archive* archive)
{
  if (jobs.size())
    return;

  if (!interpreter)
    interpreter = standard_shell();

  if (verbose)
    cerr << application_name << ": preprocessing "
	 << archive->get_filename() << endl;

  interpreter->set (archive);
  interpreter->script (jobs);
}
