#include <iostream>

#include "ArchiveProcessor.h"

int main (int argc, char** argv)
{
  Pulsar::ArchiveProcessor processor (argc, argv);

  cout << processor.prompt;

  while (!processor.quit)  {
    string command;
    getline (cin, command);
    cout << processor.parse (command);
  }

  return 0;
}
