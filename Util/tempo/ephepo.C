// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar ephemeris, update its parameters to a new epoch
//
// /////////////////////////////////////////////////////////////////////

#include <iostream>

#include "psrephem.h"

void usage()
{
  cout << "\n"
    "ephepo: update the epoch in a TEMPO ephemeris\n"
    "\n"
    "  -v        verbose\n"
       << endl;
}

int main (int argc, char ** argv)
{
  bool binary = false;
  bool verbose = false;
  int gotc = 0;

  MJD mjd;

  while ((gotc = getopt(argc, argv, "bhvm:")) != -1) {
    switch (gotc) {

    case 'b':
      binary = true;
      break;

    case 'm':
      mjd = MJD(optarg);
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
    cerr << "Please provide pulsar .par as the last argument" << endl;
    return -1;
  }

  if (mjd == 0.0) {
    cerr << "Please specify the MJD with -m" << endl;
    return -1;
  }

  psrephem eph (argv[optind]);

  eph.set_epoch (mjd, binary);

  cout << eph << endl;

  return 0;

}
