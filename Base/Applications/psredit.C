#include "Pulsar/ArchiveTUI.h"
#include "Pulsar/Archive.h"

#include "dirutil.h"
#include "string_utils.h"

#include <unistd.h>

void usage ()
{
  cout <<
    "A program for getting and setting Pulsar::Archive attributes\n"
    "Usage:\n"
    "     psredit -c CMD [-c CMD ...] filenames\n"
    "Where:\n"
    "\n"
    "CMDS is a string containing one or more commands, separated by commas\n"
    "     and/or whitespace.  If any whitespace is included, then the list\n"
    "     of commands must be enclosed in quotation marks.\n"
    "     Multiple commands may also be specified by multiple -c options.\n"
    "\n"
    "     A command can either get or a set a parameter value\n"
    "\n"
    "-------------------------------------------------\n"
    "                    PARAMETERS                   \n"
    "-------------------------------------------------\n"
    "\n"
       << endl;
}


int main (int argc, char** argv) try {  
  
  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);
  bool verbose = false;

  vector <string> commands;

  int gotc;
  while ((gotc = getopt (argc, argv, "c:hvV")) != -1)
    switch (gotc) {

    case 'c': {
      char whitespace[5] = " ,\n\t";
      char* cmd = strtok (optarg, whitespace);
      while (cmd) {
        commands.push_back(cmd);
        cmd = strtok (NULL, whitespace);
      }
      break;
    }

    case 'h':
      usage ();
      return -1;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return -1;
    }
  
  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  Pulsar::ArchiveTUI tui;

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load(filenames[ifile]);

    tui.set_instance (archive);

    for (unsigned j = 0; j < commands.size(); j++)
      tui.process (commands[j]);

  } // for each archive

  catch (Error& error) {
    if (verbose)
      cerr << error << endl;
  }

  return 0;

}
catch (Error& error) {
  cerr << "psredit: " << error << endl;
  return -1;
}
