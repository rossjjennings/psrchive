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
  metafile   = NULL;
  verbose    = false;
  has_manual = false;
}

//! Add a feature to the application
void Pulsar::Application::add (Feature* f)
{
  f->application = this;
  features.push_back( f );
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
    "\n"
    " -h               help page \n"
    " -q               quiet mode \n"
    " -v               verbose mode \n"
    " -V               very verbose mode \n"
    "\n"
    " -M metafile      metafile contains list of archive filenames \n"
       << endl;

  for (unsigned i=0; i<features.size(); i++)
    if (features[i]->get_usage().length())
      cout << features[i]->get_usage() << endl;

  if (get_usage().length())
    cout << get_usage () << endl;

  if (!has_manual)
    exit (0);

  cout << 
    "See "PSRCHIVE_HTTP"/manuals/" + name + " for more details \n" 
       << endl;

  exit (0);
}


//! Parse the command line options
void Pulsar::Application::parse (int argc, char** argv)
{
  string args = "hM:qvV";

  for (unsigned i=0; i<features.size(); i++)
    args += features[i]->get_options();

  args += get_options ();

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
      {
	bool parsed = false;
	
	for (unsigned i=0; i<features.size(); i++)
	  if (features[i]->parse (code, optarg))
	    {
	      parsed = true;
	      break;
	    }
	
	if (!parsed)
	  parsed = parse (code, optarg);
	
	if (parsed)
	  break;
	
	throw Error (InvalidParam, name,
		     "option -%c not understood", code);
      } 
      
    }

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

  for (unsigned i=0; i<features.size(); i++)
    features[i]->setup ();

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load (filenames[ifile]);

    for (unsigned i=0; i<features.size(); i++)
      features[i]->process (archive);

    process (archive);
  }
  catch (Error& error) {
    cerr << name << ": error while processing "
	 << filenames[ifile] << ":\n" << error.get_message() << endl;
  }

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
bool Pulsar::Application::parse (char code, const string& arg)
{
  return false;
}


void Pulsar::Application::Feature::process (Archive*)
{
}


void Pulsar::Application::Feature::setup ()
{
}


