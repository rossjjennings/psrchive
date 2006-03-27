/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Interpreter.h"
#include "Pulsar/Archive.h"

using namespace std;

int main (int argc, char** argv)
{
  Pulsar::Interpreter processor (argc, argv);

  if (argc == 1) {

    // no arguments: interactive mode
    processor.initialize_readline ("psrsh");
    processor.set_reply( true );
    
    while (!processor.quit)
      cout << processor.parse( processor.readline() );

  }
  else if (argc == 2) {

    // one argument: undefined mode
    cerr << "psrsh: specify script followed by archive filename[s]" << endl;
    return -1;

  }
  else {

    // two or more arguments: script mode

    // the first argument is the script to execute
    string script = argv[1];

    // all subsequent arguments are the archive filenames
    for (int i=2; i<argc; i++) try {
      processor.set( Pulsar::Archive::load (argv[i]) );
      processor.script( script );
    }
    catch (Error& error) {
      cerr << "psrsh: error while processing " << argv[i] << ":\n"
	   << error.get_message() << "\n"
	"script = " << script << endl;
    }

  }
  return 0;
}
