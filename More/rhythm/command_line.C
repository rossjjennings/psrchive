#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rhythm.h"
#include "qt_editParams.h"
#include "tempo++.h"

static char* cl_args = "ahp:t:vV";

void Rhythm::command_line_help ()
{
  cerr << "Command line help is not currently available" << endl;
}

void Rhythm::command_line_parse (int argc, char** argv)
{
  string eph_filename;

  int optc;
  while ((optc = getopt (argc, argv, cl_args)) != -1)  {
    switch (optc) {

    case 'a':
      cerr << "Rhythm: automatic fitting disabled" << endl;
      autofit = false;
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
      cerr << "rhythm: verbose on" << endl;
      verbose = 1;
      break;

    case 'V':
      cerr << "rhythm: very verbose on" << endl;
      verbose = 1;
      vverbose = 1;
      qt_fileParams::verbose = 1;
      qt_editParams::verbose = 1;
      xmp_manager::verbose = 1;
      xyplot::verbose = 1;
      Tempo::verbose = 1;

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

  if (!tim_filename.empty())
    load_toas (tim_filename.c_str());

  if (!eph_filename.empty()) {
    if (verbose && !vverbose)
      cerr << "Loading TEMPO Parameters from '" << eph_filename << "'" << endl;
    fitpopup -> load (eph_filename.c_str());
    cerr << "Parameters Loaded." << endl;
  }
  else {
    fitpopup -> open ();
  }

}
