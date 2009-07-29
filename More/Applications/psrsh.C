/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/psrchive.h"
#include "Pulsar/Interpreter.h"

#include "Pulsar/Archive.h"

#include "strutil.h"
#include "dirutil.h"

#include <unistd.h>

using namespace std;

void usage (const string& script)
{
  if (!script.length()) cout << "\n"
    "psrsh - PSRCHIVE command language interpreter \n"
    "\n"
    "usage: psrsh [options] [script filename[s]] \n";
  else cout <<
    script << " - a PSRCHIVE script \n"
    "\n"
    "usage: " << script << " [options] filename[s] \n";
  cout <<
    "options:\n"
    " -h               this help page \n"
    " -H               list available commands \n"
    " -M metafile      metafile contains list of archive filenames \n"
    " -q               quiet mode \n"
    " -v               verbose mode \n"
    " -V               very verbose mode \n"
    "\n";
  if (!script.length()) cout <<
    "If no files are specified, psrsh enters the interactive shell mode \n"
    "\n"
    "Otherwise, psrsh enters the shell script command processor mode: \n"
    "the first file must be the script, and all subseqent archive files \n"
    "will be processed using this script \n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/psrsh for more details \n" << endl;
}



int main (int argc, char** argv)
{
  // name of file containing list of Archive filenames
  char* metafile = NULL;
  // help requested
  bool help = false;
  //
  bool verbose = false;

  Pulsar::Interpreter* interpreter = standard_shell();

  char c;
  while ((c = getopt(argc, argv, "hHM:qvV")) != -1) 

    switch (c)  {

    case 'h':
      help = true;
      break;

    case 'H':
      cout << interpreter->help("") << endl;
      return 0;

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

    } 

  string script;
  if (optind < argc) {
    script = argv[optind];
    optind ++;
  }

  if (help) {
    usage (script);
    return 0;
  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty()) {

    if (!script.empty()) {
      cout << "psrsh: please specify filename[s]" << endl;
      return -1;
    }

    // no arguments: interactive mode
    interpreter->initialize_readline ("psrsh");
    interpreter->set_reply( true );
    
    while (!interpreter->quit)
      cout << interpreter->parse( interpreter->readline() );

  }
  else {

    if (script.empty()) {
      cout << "psrsh: please specify script" << endl;
      return -1;
    }

    // two or more arguments: script mode

    for (unsigned i=0; i<filenames.size(); i++) try
    {
      interpreter->set( Pulsar::Archive::load (filenames[i]) );
      interpreter->script( script );
    }
    catch (Error& error)
    {
      cerr << "psrsh: error while processing " << filenames[i] << ":" << endl;
      if (verbose)
        cerr << error << endl;
      else
	cerr << error.get_message() << endl;
      cerr << "script = " << script << endl;
    }

  }
  return 0;
}
