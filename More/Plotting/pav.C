#include <iostream>
#include <unistd.h>
#include <cpgplot.h>

#include "Archive.h"
#include "Integration.h"
#include "Profile.h"
#include "Error.h"

#include "dirutil.h"
#include "string_utils.h"

void usage ()
{
  cout << "program to look at Pulsar::Archive(s) in various ways \n"
    "Usage: treduce [options] file1 [file2 ...] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -a        Calculate TOAs of every profile \n"
    " -b scr    Bscrunch scr phase bins together \n"
    " -c        Correct data for bad ephemeris \n"
    " -d dm     Dedisperse data at a new dm \n"
    " -D        Plot Integration 0, poln 0, chan 0 \n"
    " -E f.eph  install new ephemeris given in file 'f.eph' \n"
    " -e xx     Output data to new file with ext xx \n"
    " -f scr    Fscrunch scr frequency channels together \n"
    " -F        Fscrunch all frequency channels \n"
    " -H        Print ASCII of Integration 0, poln 0, chan 0 \n"
    " -m macro  Process data using steps in macro \n"
    " -M meta   meta names a file containing the list of files\n"
    " -p        add polarisations together \n"
    " -S        plot pulsar dynamic spectra:  frequency vs time. \n"
    " -t src    Tscrunch scr Integrations together \n"
    " -T        Tscrunch all Integrations \n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
    " -x nx     plot nx profiles across screen \n"
    " -y ny     ploy ny profiles down screen\n"
       << endl;
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;

  bool verbose = false;
  bool display = false;

  char* metafile = NULL;

  int c = 0;
  const char* args = "ab:cd:De:E:f:FHm:M:pSt:TvVx:y:";
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'a':
      // toas
      break;
    case 'b':
      bscrunch = atoi (optarg);
      break;
    case 'c':
      // correct
      break;
    case 'd':
      // parse dm
      break;
    case 'D':
      display = true;
      break;
    case 'e':
      // parse ext
      break;
    case 'E':
      // parse eph file
      break;
    case 'f':
      fscrunch = atoi (optarg);
      break;
    case 'F':
      fscrunch = 0;
      break;
    case 'h':
      usage ();
      return 0;
    case 'H':
      // ascii
      break;
    case 'm':
      // macro file
      break;
    case 'M':
      metafile = optarg;
      break;
    case 'p':
      pscrunch = 0;
      break;
    case 'S':
      // dynamic spectra
      break;
    case 't':
      tscrunch = atoi (optarg);
      break;
    case 'T':
      tscrunch = 0;
      break;
    case 'V':
    case 'v':
      verbose = true;
      break;
    case 'x':
      // x panel
      break;
    case 'y':
      // y panel
      break;
    default:
      cerr << "invalid param '" << c << "'" << endl;
    }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else 
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0) {
    usage ();
    return 0;
  }

  if (display)
    cpgbeg (0, "?", 0, 0);

  Pulsar::Archive* archive = 0;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::factory (filenames[ifile]);

    if (bscrunch > 0)
      archive -> bscrunch (bscrunch);

    if (fscrunch >= 0)
      archive -> fscrunch (fscrunch);

    if (tscrunch >= 0)
      archive -> tscrunch (tscrunch);

    if (pscrunch > 0)
      archive -> pscrunch ();

    if (display) 
      archive -> get_Profile(0,0,0) -> display();

    delete archive; archive = 0;
  }
  catch (Pulsar::Error& error) {
    cerr << error << endl;
    if (archive)
      delete archive; archive = 0;
  }

  if (display)
    cpgend();

  return 0;
}

