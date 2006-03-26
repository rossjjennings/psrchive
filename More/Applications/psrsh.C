/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Interpreter.h"
#include <iostream>

using namespace std;

int main (int argc, char** argv)
{
  Pulsar::Interpreter processor (argc, argv);

  cout << processor.prompt;

  while (!processor.quit)  {
    string command;
    getline (cin, command);
    cout << processor.parse (command);
  }

  return 0;
}
