#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>

#include "rhythm.h"


static char* cl_args = "hp:t:v";
void Rhythm::command_line_parse (int argc, char** argv)
{
cerr << "begin parsing command line\n";
for (int i=0; i<argc; i++)
  fprintf (stderr, "%s ", argv[i]);
cerr << endl;

  int optc;
  while ((optc = getopt (argc, argv, cl_args)) != -1)  {
    switch (optc) {

    case 'p':
      eph_filename = optarg;
      break;

    case 't':
      tim_filename = optarg;
      break;

    case 'v':
      cerr << "rhythm: verbose on\n";
      verbose = 1;
      break;

    default:
      cerr << "rhythm: invalid command line option '" << optc << "'\n";
      break;

    } // end switch (optc)
  } // end while reading options

  if (verbose)
    cerr << "Options read.  Checking for filenames.\n";

  // read any remaining options as filenames or a basename
  string basename;

  vector<string> ephext = psrephem::extensions();
  if (verbose) {
    cerr << "psrephem extensions: ";
    for (int iext=0; iext < ephext.size(); iext++)
      cerr << ephext[iext].c_str() << " ";
    cerr << endl;
  }
  for (int argi = optind; argi < argc; argi++) {
    bool match = false;

    for (int iext=0; iext < ephext.size(); iext++)
      if (strstr (argv[argi], ephext[iext].c_str())) {
	if (verbose)
	  cerr << "Ephemeris file given: '" << argv[argi] << "'\n";
	eph_filename = argv[argi];
	match = true;
	break;
      }

    if (match)
      continue;

    if (strstr (argv[argi], ".tim")) {
      if (verbose)
	cerr << "TOA file given: '" << argv[argi] << "'\n";
      tim_filename = argv[argi];
      match = true;
      continue;
    }

    // no match made, try to interpret the string as a basename
    basename = argv[argi];
  }

  if (!tim_filename.empty()) {
    if (verbose)
      fprintf (stderr, "Loading TOAs from '%s'...", tim_filename.c_str());
    toa::load (tim_filename.c_str(), &arrival_times);
    if (verbose)
      fprintf (stderr, "  loaded\n");

    if (verbose)
      fprintf (stderr, "Sorting TOAS...");
    sort (arrival_times.begin(), arrival_times.end());
    if (verbose)
      fprintf (stderr, "  sorted\n");
  }

  if (!eph_filename.empty()) {
    if (verbose)
      fprintf (stderr, "Loading ephemeris from '%s'\n", eph_filename.c_str());
    psrephem model;
    model.load (eph_filename.c_str());
    if (verbose) {
      cerr << "Ephemeris loaded:\n";
      model.unload (stderr);
    }
    ephemerides.push_back (model);
  }

  if (arrival_times.size() > 0 && ephemerides.size() > 0
      && !ephemerides.back().psrname().empty()) {

    if (verbose)
      cerr << "Calculating residuals\n";
    tempo_fit (ephemerides.back(), arrival_times, NULL, &residuals);

  }

}
