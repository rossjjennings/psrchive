/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConfigurableProjection.h"

#include <iostream>

using namespace std;

int main (int argc, char** argv) try
{
  if (argc < 2)
  {
    cerr << "please provide a yaml filename" << endl;
    return -1;
  }

  string filename = argv[1];

  auto cpc = new Pulsar::ConfigurableProjection (filename);
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}

