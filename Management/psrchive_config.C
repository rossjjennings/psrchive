/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This simple program outputs a configuration file containing all current
  configuration parameters and their default values.
*/

#include "Pulsar/psrchive.h"
#include "Pulsar/Interpreter.h"

using namespace std;

int main (int argc, char** argv)
{
  // this simply ensures that all symbols are loaded
  Pulsar::Interpreter* interpreter = standard_shell();


  return 0;
}
