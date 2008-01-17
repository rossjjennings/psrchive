/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"

using namespace std;

class psrsp : public Pulsar::Application
{
public:
  psrsp ();
  void process (Pulsar::Archive*);
};

psrsp::psrsp () : Application ("psrsp", "pulsar spectral power")
{
  add( new Pulsar::StandardOptions );
}

void psrsp::process (Pulsar::Archive*)
{
}

int main (int argc, char** argv)
{
  psrsp program;

  return program.main (argc, argv);
}

