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

Pulsar::Application::Application (const string& n, const string& d)
{
  name = n;
  description = d;

  metafile     = NULL;
  has_manual   = false;

  verbose      = false;
  very_verbose = false;
}

//! Add options to the application
void Pulsar::Application::add (Options* f)
{
  f->application = this;
  options.push_back( f );
}

//! Get the application name
std::string Pulsar::Application::get_name () const
{
  return name;
}

//! Get the application description
std::string Pulsar::Application::get_description () const
{
  return description;
}

//! Get the verbosity flag
bool Pulsar::Application::get_verbose () const
{
  return verbose;
}

//! Provide usage information
void Pulsar::Application::usage ()
{
  cout << 
    "\n" + name + " - " + description + "\n"
    "\n"
    "usage: " + name + " [options] filename[s] \n"
    "\n"
    "where options are:\n"
       << endl;

  if (!version.empty())
    cout << " -i               revision information \n";

  cout <<
    " -h               help page \n"
    " -q               quiet mode \n"
    " -v               verbose mode \n"
    " -V               very verbose mode \n"
    "\n"
    " -M metafile      metafile contains list of archive filenames \n"
       << endl;

  for (unsigned i=0; i<options.size(); i++)
    if (options[i]->get_usage().length())
      cout << options[i]->get_usage() << endl;

  if (get_usage().length())
    cout << get_usage () << endl;

  if (!has_manual)
    exit (0);

  cout << 
    "See "PSRCHIVE_HTTP"/manuals/" + name + " for more details \n" 
       << endl;
}


//! Parse the command line options
void Pulsar::Application::parse (int argc, char** argv)
{
  string args = "hM:qvV";

  if (!version.empty())
    args += "i";

  for (unsigned i=0; i<options.size(); i++)
    args += options[i]->get_options();

  args += get_options ();

  char code;
  while ((code = getopt(argc, argv, args.c_str())) != -1) 
  {    
    if (filter)
      code = filter (code);

    switch (code)  {

    case 'i':
      cout << version << endl;
      exit (0);

    case 'h':
      usage ();
      exit (0);

    case 'M':
      metafile = optarg;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      set_quiet ();
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      set_verbose ();
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      verbose = true;
      very_verbose = true;
      set_very_verbose ();
      break;

    default:
      {
	bool parsed = false;
        std::string arg = "";

        if (optarg!=NULL) arg = optarg;
	
	for (unsigned i=0; i<options.size(); i++)
	  if (options[i]->parse (code, arg))
	    {
	      parsed = true;
	      break;
	    }
	
	if (!parsed)
	  parsed = parse (code, arg);
	
	if (parsed)
	  break;
	
	throw Error (InvalidParam, name,
		     "option -%c not understood", code);
      } 
    }
  }

  dirglob_program = name;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
    throw Error (InvalidParam, name,
		 "please specify filename[s]");
}

//! Execute the main loop
int Pulsar::Application::main (int argc, char** argv) try
{
  parse (argc, argv);

  for (unsigned i=0; i<options.size(); i++)
    options[i]->setup ();

  setup ();

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load (filenames[ifile]);

    for (unsigned i=0; i<options.size(); i++)
    {
      if (very_verbose)
	cerr << "Pulsar::Application::main feature "<< i <<" process" << endl;
      options[i]->process (archive);
    }

    process (archive);

    for (unsigned i=0; i<options.size(); i++)
    {
      if (very_verbose)
	cerr << "Pulsar::Application::main feature "<< i <<" finish" << endl;
      options[i]->finish (archive);
    }

  }
  catch (Error& error)
  {
    cerr << name << ": error while processing " << filenames[ifile] << ":";
    if (verbose)
      cerr << error << endl;
    else
      cerr << "\n" << error.get_message() << endl;
  }

  for (unsigned i=0; i<options.size(); i++)
    options[i]->finalize ();

  finalize ();

  return 0;
}
catch (Error& error)
{
  cerr << name << ": " << error.get_message() << endl;
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
bool Pulsar::Application::parse (char code, const std::string& arg)
{
  return false;
}

//! Extra setup, run once before main loop
void Pulsar::Application::setup ()
{
}

//! Return true if application must save data
bool Pulsar::Application::must_save ()
{
  return true;
}

//! Extra optional processing tasks, run once per Archive (before main process)
void Pulsar::Application::Options::process (Archive*)
{
}

//! Final steps, run once at end of program
void Pulsar::Application::finalize ()
{
}

//! Optional final steps, run once at end
void Pulsar::Application::Options::finalize ()
{
}

//! Optional setup steps, run once at start
void Pulsar::Application::Options::setup ()
{
}

//! Optional finishing tasks, run once per Archive (after main process)
void Pulsar::Application::Options::finish (Archive*)
{
}

