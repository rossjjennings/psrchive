/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar ephemeris and MJD, computes binary phase
//
// interface to routines in psrephem_orbital.C by Aidan Hotan
//
// /////////////////////////////////////////////////////////////////////

#include "psrephem_orbital.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>

using namespace std;
using Legacy::psrephem;

void usage()
{
  cout << "\n"
    "ephorb: compute binary phase\n"
    "\n"
    "usage: ephorb [options] <pulsar.eph>\n"
    "\n"
    "where options are:\n"
    "\n"
    "  -m MJD   MJD [default: now] \n"
    "  -s char  site code from $TEMPO/obsys.dat \n"
    "  -f freq  observing frequency \n"
    "\n"
    "  -p       binary phase wrt periastron \n"
    "  -P       binary longitude wrt periastron \n"
    "  -a       binary phase wrt ascending node \n"
    "  -A       binary longitude wrt ascending node \n"
       << endl;
}

int main (int argc, char ** argv) try
{
  bool phase_periastron = false;
  bool longitude_periastron = false;
  bool phase_ascending = false;
  bool longitude_ascending = false;

  MJD mjd;
  char site = '7';
  double freq = 1400.0;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hf:m:s:pPaA")) != -1) {
    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'f':
      freq = atof (optarg);
      break;

    case 'm':
      mjd = MJD(optarg);
      break;

    case 's':
      site = optarg[0];
      break;

    case 'p':
      phase_periastron = true;
      break;

    case 'P':
      longitude_periastron = true;
      break;

    case 'a':
      phase_ascending = true;
      break;

    case 'A':
      longitude_ascending = true;
      break;

    }
  }

  if (optind >= argc)
  {
    cerr << "Please provide tempo parameter file" << endl;
    return -1;
  }

  if (mjd == 0.0)
  {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    fprintf (stderr, "Using current date/time: %s\n", asctime(&date));
    mjd = MJD (date);
  }

  psrephem eph (argv[optind]);

  double epoch = mjd.in_days();

  if ( phase_periastron )
    cout << "periastron phase = " 
	 << get_binphs_peri (epoch, eph, freq, site)*360.0 << " deg" << endl;
  
  if ( longitude_periastron )
    cout << "periastron longitude = " 
	 << get_binlng_peri (epoch, eph, freq, site) << " deg" << endl;
  
  if ( phase_ascending )
    cout << "ascending phase = " 
	 << get_binphs_asc (epoch, eph, freq, site)*360.0 << " deg" << endl;
  
  if ( longitude_ascending )
    cout << "ascending longitude = " 
	 << get_binlng_asc (epoch, eph, freq, site) << " deg" << endl;
  
  return 0;
}
catch (Error& error)
{
  cerr << "ephorb: error" << error << endl;
  return -1;
}
