/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardApplication.h"
#include "Pulsar/Archive.h"

using namespace std;

class psrsp : public Pulsar::StandardApplication
{
public:
  psrsp ();
  void process (Pulsar::Archive*);
};

psrsp::psrsp () : StandardApplication ("psrsp", "pulsar spectral power")
{
}

void psrsp::process (Pulsar::Archive*)
{
}

int main (int argc, char** argv)
{
  psrsp program;

  return program.main (argc, argv);
}

