#include "Pulsar/ArchiveTUI.h"
#include "Pulsar/ReceiverTUI.h"

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
    "CMDS is a string containing one or more commands, separated by commas.\n"
    "     If any whitespace is required, then the string containing it must\n"
    "     be enclosed in quotation marks.  Multiple commands may also be\n"
    "     specified by using multiple -c options.\n"
    "\n"
    "     A command can either get or a set a parameter value.\n"
    "     A get command is simply the name of the parameter.\n"
    "     A set command is a parameter assignment statement, e.g\n"
    "\n"
    "     psredit -c freq,NAME=\"Hydra A\"\n"
    "\n"
    "     will print the centre frequency and set the source name.\n"
    "     Note that parameter names are case insensitive.\n"
    "     For the full list of parameter names, type \"psredit -H\"\n"
       << endl;
}

string pad (unsigned length, string text)
{
  while (text.length() < length)
    text += " ";
  return text;
}

int main (int argc, char** argv) try {  
  
  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);
  bool verbose = false;

  vector <string> commands;

  Pulsar::ArchiveTUI tui;

  Pulsar::ReceiverTUI receiver_tui;
  tui.import( "rcvr", &receiver_tui );

  int gotc;
  while ((gotc = getopt (argc, argv, "c:hHvV")) != -1)
    switch (gotc) {

    case 'c': {
      char whitespace[5] = " ,\n\t";
      char* cmd = strtok (optarg, whitespace);
      while (cmd) {
        if (verbose)
          cerr << "psredit: parsed command '" << cmd << "'" << endl;
        commands.push_back(cmd);
        cmd = strtok (NULL, whitespace);
      }
      break;
    }

    case 'h':
      usage ();
      return 0;

    case 'H': {

      cout << 
	"-------------------------------------------------\n"
	"Attribute Name   Description\n"
	"-------------------------------------------------"
	   << endl;

      for (unsigned i=0; i<tui.get_nattribute(); i++)
	cout << pad(16,tui.get_name(i)) <<" "<< tui.get_description(i) << endl;

      return 0;

    }

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

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load(filenames[ifile]);

    cout << archive->get_filename();

    tui.set_instance (archive);

    for (unsigned j = 0; j < commands.size(); j++)  {
      if (verbose)
        cerr << "psredit: processing command '" << commands[j] << "'" << endl;
      tui.process (commands[j]);
    }

    cout << endl;

  } // for each archive

  catch (Error& error) {
    cout << " " << error.get_message() << endl;
  }

  return 0;

}
catch (Error& error) {
  cerr << "psredit: " << error << endl;
  return -1;
}
