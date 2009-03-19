/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Check.h"
#include "Pulsar/Profile.h"

#include "TextInterface.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>

using namespace std;
using Pulsar::Archive;

void usage ()
{
  cout <<
    "A program for getting and setting Pulsar::Archive attributes\n"
    "Usage:\n"
    "     psredit -c CMD [-c CMD ...] filenames\n"
    "Where:\n"
    "\n"
    "CMD is a string containing one or more commands, separated by commas.\n"
    "    If any whitespace is required, then the string containing it must\n"
    "    be enclosed in quotation marks.  Multiple commands may also be\n"
    "    specified by using multiple -c options.\n"
    "\n"
    "    A command can either get or a set a parameter value.\n"
    "    A get command is simply the name of the parameter.\n"
    "    A set command is a parameter assignment statement, e.g\n"
    "\n"
    "    psredit -c freq,name=\"Hydra A\"\n"
    "\n"
    "    will print the centre frequency and set the source name.\n"
    "    Note that parameter names are case insensitive.\n"
    "\n"
    "    For the complete list of all available parameters in an archive, \n"
    "    run \"psredit <filename>\" without any command line options. \n"
       << endl;
}

const char* long_help = 
"\n"
"PLEASE NOTE: the list of available parameters is generated dynamically.\n"
"\n"
"To list all available parameters, simply load an archive file without \n"
"specifying any command line options. \n"
"\n"
"Parameter names followed by a '*' represent vectors; these may be further \n"
"specified using range notation.  For example:\n"
"\n"
"  psredit -c int[0,8-15]:mjd\n"
"\n"
"will print the epoch from sub-integrations 0 and 8 through 15 inclusive.\n"
"Output values will be separated by a comma.  If a range is not specified,\n"
"all values will be output.  Note that range indeces begin at zero.\n";

using namespace Pulsar;

int main (int argc, char** argv) try
{  
  // print/parse in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Archive::set_verbosity (0);

  // disable sanity checks that try to load Integration data
  Archive::Check::disable ("Dedispersed");
  Archive::Check::disable ("DeFaradayed");

  bool verbose = false;

  vector <string> commands;

  // print the name of each file processed
  bool output_filename = true;

  // prefix parameter value queries with parameter name=
  bool prefix_name = true;

  // save processed files
  bool save = false;

  // save with a new extension
  string save_ext;

  int gotc;
  while ((gotc = getopt (argc, argv, "c:e:hHmqQvV")) != -1)
    switch (gotc) {

    case 'c':
      separate (optarg, commands, ",");
      break;

    case 'e':
      save = true;
      save_ext = optarg;
      break;

    case 'h':
      usage ();
      return 0;

    case 'H':
      cerr << long_help << endl;
      return 0;

    case 'm':
      save = true;
      break;

    case 'q':
      output_filename = false;
      break;

    case 'Q':
      prefix_name = false;
      break;

    case 'v':
      verbose = true;
      Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Error::verbose = true;
      Archive::set_verbosity(3);
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return -1;
    }

  bool edit = false;
  for (unsigned j = 0; j < commands.size(); j++)
    if (commands[j].find('=') != string::npos)
      edit = true;

  if (edit && !save) {
    cerr << "psredit: changes will not be saved."
      " Use -m or -e to write results to disk" << endl;
  }

  if (!save)
    // load files quickly (no data)
    Pulsar::Profile::no_amps = true;

  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try
  {
    Reference::To<Archive> archive;
    archive = Archive::load(filenames[ifile]);

    Reference::To<TextInterface::Parser> interface = archive->get_interface();

    if (commands.size() == 0)
    {
      cout << interface->help (true) << endl;;
      continue;
    }

    // so that a space precedes each parameter processed
    interface->set_indentation (" ");
    interface->set_prefix_name (prefix_name);

    if (output_filename)
      cout << archive->get_filename();

    for (unsigned j = 0; j < commands.size(); j++)
    {
      if (verbose)
        cerr << "psredit: processing command '" << commands[j] << "'" << endl;

      cout << interface->process (commands[j]);
    }

    cout << endl;

    if (edit && save)
    {
      if (save_ext.empty())
      {
	archive->unload();
	cout << archive->get_filename() << " updated on disk" << endl;
      }
      else
      {
	string name = replace_extension( archive->get_filename(), save_ext );
	archive->unload(name);
	cout << archive->get_filename() << " written to disk" << endl;
      }
    }
  } // for each archive

  catch (Error& error)
  {
    if (verbose)
      cerr << "psredit: error on " << filenames[ifile] << error << endl;
    else
      cerr << " " << error.get_message() << endl;
  }

  return 0;
}
catch (Error& error)
{
  cerr << "psredit: " << error << endl;
  return -1;
}
