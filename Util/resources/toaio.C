/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// /////////////////////////////////////////////////////////////////////
//
// Load TOAs and unload in the specified format
//
// /////////////////////////////////////////////////////////////////////

#include "toa.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>

using namespace std;

void usage()
{
  cout << "\n"
    "toaio: update the epoch in a TEMPO ephemeris\n"
    "\n"
    "  -f fmt  set output TOA format\n"
    "  -v      verbose\n"
       << endl;
}

int main (int argc, char ** argv)
{
  Tempo::toa::Format format = Tempo::toa::Unspecified;
  bool verbose = false;
  int gotc = 0;

  while ((gotc = getopt(argc, argv, "hvf:")) != -1) {
    switch (gotc) {

    case 'f':
      format = fromstring<Tempo::toa::Format>( optarg );
      break;

    case 'h':
      usage ();
      return 0;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (optind >= argc) {
    cerr << "Please specify TOA input file" << endl;
    return -1;
  }

  std::vector<Tempo::toa> toas;
  
  Tempo::toa::load (argv[optind], &toas);
  
  if (toas.size() <= 0) {
    cerr << "toaio: no TOAs loaded from " << argv[optind] << endl;
    return -1;
  }

  Tempo::toa::unload (stdout, toas, format);
  return 0;
}
