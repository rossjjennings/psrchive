#define PGPLOT 1

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/Archive.h"

#include "string_utils.h"
#include "dirutil.h"

#include <iostream>
#include <unistd.h>


void usage ()
{
  cerr << "pacv - Pulsar Archive Calibrator Viewer\n"
    "usage: pacv file1 [file2 ...]" << endl;
}

int main (int argc, char** argv) 
{
  char* metafile = NULL;

  bool verbose = false;
  char c;
  while ((c = getopt(argc, argv, "hvV")) != -1)  {

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      Pulsar::Calibrator::verbose = true;
    case 'v':
      Error::verbose = true;
      verbose = true;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "psrcal requires a list of archive filenames as parameters.\n";
    return -1;
  }

  vector <string> filenames;
  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  cpgbeg (0, "?", 0, 0);
  cpgask(1);

  cpgsvp (0.1,.95, 0.1,.95);
  
  // the calibrator archive
  Reference::To<Pulsar::Archive> archive;
 
  Reference::To<Pulsar::SingleAxisCalibrator> calibrator;

  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {  try {

    archive = Pulsar::Archive::load( filenames[ifile] );

    calibrator = new Pulsar::SingleAxisCalibrator (archive);

    Pulsar::plot (calibrator);

  } catch (Error& error) {
    cerr << "psrcal: Error calibrating " << filenames[ifile] << error << endl;
    return -1;
  }
  }

  return 0;
}
