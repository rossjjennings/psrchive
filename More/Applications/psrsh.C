#include <iostream>

#include "ArchiveProcessor.h"

int main (int argc, char** argv)
{
  Pulsar::ArchiveProcessor processor (argc, argv);

  // processor.verbose = true;
  string command;

  while (!processor.quit)  {

    getline (cin, command);

    if ( processor.execute (command) < 0 )
      cerr << processor.get_status() << endl;

  }

  return 0;
}
