#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "qt_editParams.h"
#include "rhythm.h"

static char* cl_args = "ahp:t:vV";

void Rhythm::command_line_help ()
{
  cerr << "Command line help is not currently available" << endl;
}

void Rhythm::command_line_parse (int argc, char** argv)
{
  char* eph_filename = NULL;
  char* tim_filename = NULL;

  int optc;
  while ((optc = getopt (argc, argv, cl_args)) != -1)  {
    switch (optc) {

    case 'a':
      toglauto();
      break;

    case 'h':
      command_line_help ();
      break;

    case 'p':
      eph_filename = optarg;
      break;

    case 't':
      tim_filename = optarg;
      break;

    case 'v':
      setVerbosity (mediumID);
      break;

    case 'V':
      setVerbosity (noisyID);
      break;

    default:
      cerr << "rhythm: invalid command line option '" << char(optc) << "'\n";
      break;

    } // end switch (optc)
  } // end while reading options

  if (verbose)
    cerr << "Options read.  Checking for filenames.\n";

  // read any remaining options as filenames or a basename
  string basename;

  vector<string> ephext = psrParams::extensions();
  if (vverbose) {
    cerr << "psrParams extensions: ";
    for (unsigned iext=0; iext < ephext.size(); iext++)
      cerr << ephext[iext].c_str() << " ";
    cerr << endl;
  }
  for (int argi = optind; argi < argc; argi++) {
    bool match = false;

    for (unsigned iext=0; iext < ephext.size(); iext++)
      if (strstr (argv[argi], ephext[iext].c_str())) {
	if (vverbose)
	  cerr << "Ephemeris file given: '" << argv[argi] << "'\n";
	eph_filename = argv[argi];
	match = true;
	break;
      }

    if (match)
      continue;

    if (strstr (argv[argi], ".tim")) {
      if (vverbose)
	cerr << "TOA file given: '" << argv[argi] << "'\n";
      tim_filename = argv[argi];
      match = true;
      continue;
    }

    // no match made, try to interpret the string as a basename
    basename = argv[argi];
  }

  if (tim_filename)
    load_toas (tim_filename);

  if (eph_filename) {
    if (verbose && !vverbose)
      cerr << "Loading TEMPO Parameters from '" << eph_filename << "'" << endl;
    fitpopup -> load (eph_filename);
    if (verbose && !vverbose)
      cerr << "Parameters Loaded." << endl;
  }
  else {
    fitpopup -> open ();
  }

}
