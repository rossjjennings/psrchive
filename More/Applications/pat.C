#include <string.h>
#include <libgen.h>
#include "dirutil.h"

#include "cpgplot.h"

#include "Pulsar/getopt.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Plotter.h"
#include "Error.h"
#include "Pulsar/Profile.h"
#include "Phase.h"
#include "toa.h"

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  bool std_given = false;
  bool time_domain = false;

  string std;

  vector<string> archives;
  vector<Tempo::toa> toas;

  int gotc = 0;

  while ((gotc = getopt(argc, argv, "hvVis:t")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for timing Pulsar::Archives"                             << endl;
      cout << "Usage: pat [options] filenames"                                    << endl;
      cout << "  -v               Verbose mode"                                   << endl;
      cout << "  -V               Very verbose mode"                              << endl;
      cout << "  -i               Show revision information"                      << endl;
      cout << "  -s stdfile       Location of standard profile"                   << endl;
      cout << "  -t               Fit in the time domain"                         << endl;
      cout << endl;
      cout << "See http://astronomy.swin.edu.au/pulsar/software/manuals/pat.html" << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'i':
      cout << "$Id: pat.C,v 1.14 2003/12/03 07:54:53 ahotan Exp $" << endl;
      return 0;
    case 's':
      std_given = true;
      std = optarg;
      break;
    case 't':
      time_domain = true;
      break;
    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  } 
  
  if (!std_given) {
    cerr << "You must specify the standard profile to use!" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> arch;
  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> prof;

  try {
    
    stdarch = Pulsar::Archive::load(std);
    stdarch->fscrunch();
    stdarch->tscrunch();
    stdarch->convert_state(Signal::Intensity);

  }
  catch (Error& error) {
    cerr << "Error processing standard profile:" << endl;
    cerr << error << endl;
    return -1;
  }

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);

      arch->convert_state (Signal::Intensity);      
      arch->toas(toas, stdarch, time_domain);

      for (unsigned i = 0; i < toas.size(); i++)
	toas[i].unload(stdout);

    }
    catch (Error& error) {
      fflush(stdout);
      cerr << error << endl;
    }
  }
  fflush(stdout);
}


