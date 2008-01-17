/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/psrchive.h"

#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>

Pulsar::Application::Application (const string& name,
				  const string& description)
{
  application_name = name;
  application_description = description;
  metafile   = NULL;
  verbose    = false;
  has_manual = false;
}

//! Provide usage information
void Pulsar::Application::usage ()
{
  cout << 
    "\n" + application_name + " - " + application_description + "\n"
    "\n"
    "usage: " + application_name + " [options] filename[s] \n"
    "\n"
    "where options are:\n"
    "\n"
    " -h               help page \n"
    " -q               quiet mode \n"
    " -v               verbose mode \n"
    " -V               very verbose mode \n"
    "\n"
    " -M metafile      metafile contains list of archive filenames \n"
    "\n"
       << get_usage () 
       << endl;

  if (!has_manual)
    exit (0);

  cout << 
    "See "PSRCHIVE_HTTP"/manuals/" + application_name + " for more details \n" 
       << endl;

  exit (0);
}

//! Parse the command line options
void Pulsar::Application::parse (int argc, char** argv)
{
  string args = "hM:qvV" + get_options ();

  char code;
  while ((code = getopt(argc, argv, args.c_str())) != -1) 

    switch (code)  {

    case 'h':
      usage ();
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      verbose = true;
      break;

    default:
      if (! parse (code, optarg) )
	throw Error (InvalidParam, application_name,
		     "option -%c not understood", code);
      break;

    } 

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
    throw Error (InvalidParam, application_name,
		 "please specify filename[s]");
}

//! Execute the main loop
int Pulsar::Application::main (int argc, char** argv) try
{
  parse (argc, argv);

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load (filenames[ifile]);

    preprocess (archive);
    process (archive);
  }
  catch (Error& error) {
    cerr << application_name << ": error while processing "
	 << filenames[ifile] << ":\n" << error.get_message() << endl;
  }

  return 0;
}
catch (Error& error)
{
  cerr << application_name << ": " << error.get_message() << endl;
  return -1;
}

//! Extra usage information implemented by derived classes
string Pulsar::Application::get_usage ()
{
  return "";
}

//! Extra option flags implemented by derived classes
string Pulsar::Application::get_options ()
{
  return "";
}

//! Parse a non-standard command
bool Pulsar::Application::parse (char code, const string& arg)
{
  return false;
}

//! Preprocessing tasks implemented by partially derived classes
void Pulsar::Application::preprocess (Archive*)
{
}


