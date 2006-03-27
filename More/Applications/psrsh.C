/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Interpreter.h"

using namespace std;

int main (int argc, char** argv)
{
  Pulsar::Interpreter processor (argc, argv);

  while (!processor.quit)
    cout << processor.parse( processor.readline() );

  return 0;
}
