#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include "cpgplot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Plotter.h"
#include "Error.h"
#include "Pulsar/Profile.h"
#include "toa.h"

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  bool std_given = false;

  string std;

  vector<string> archives;
  vector<Tempo::toa> toas;

  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVs:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for timing Pulsar::Archives"            << endl;
      cout << "Usage: pat [options] filenames"                   << endl;
      cout << "  -v               Verbose mode"                  << endl;
      cout << "  -V               Very verbose mode"             << endl;
      cout << "  -s [path]        Location of standard profile"  << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 's':
      std_given = true;
      std = optarg;
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

  toas.resize(archives.size());

  Reference::To<Pulsar::Archive> arch;
  Reference::To<Pulsar::Archive> total;
  Reference::To<Pulsar::Profile> prof;
  Reference::To<Pulsar::Profile> stdp;

  MJD start_time;
  double period;

  try {
    
    arch = Pulsar::Archive::load(std);
    total = arch->clone();
    total->fscrunch();
    total->tscrunch();
    total->centre();
    stdp = total->get_Profile(0,0,0);

  }
  catch (Error& error) {
    cerr << error << endl;
    return -1;
  }

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);
      total = arch->clone();
      total->fscrunch();
      total->tscrunch();
      total->centre();

      prof = total->get_Profile(0,0,0);
      start_time = arch->start_time();
      period = arch->get_Integration(0)->get_folding_period();
      
      toas[i] = prof->toa(stdp.get(), start_time, period,
			  total->get_telescope_code());
    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }

  for (unsigned i = 0; i < toas.size(); i++)
    toas[i].unload(stdout);
  
}


