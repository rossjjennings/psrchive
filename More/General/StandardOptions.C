/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/StandardOptions.h"
#include "Pulsar/Interpreter.h"

#include "strutil.h"

Pulsar::StandardOptions::StandardOptions ()
{
}

//! Extra usage information implemented by derived classes
std::string Pulsar::StandardOptions::get_usage ()
{
  return 
    " -j commands      execute pulsar shell preprocessing commands \n"
    " -J script        execute pulsar shell preprocessing script \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::StandardOptions::get_options ()
{
  return "j:J:";
}

//! Parse a non-standard command
bool Pulsar::StandardOptions::parse (char code, const std::string& arg)
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
void Pulsar::StandardOptions::process (Archive* archive)
{
  if (jobs.size() == 0)
    return;

  if (!interpreter)
    interpreter = standard_shell();

  if (application->get_verbose())
    cerr << application->get_name() << ": interpreter processing "
	 << archive->get_filename() << endl;

  interpreter->set (archive);
  interpreter->script (jobs);
}
