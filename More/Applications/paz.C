#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include "cpgplot.h"

#include "Pulsar/Integration.h"
#include "Pulsar/Plotter.h"
#include "Error.h"
#include "Pulsar/RFIMitigation.h"

// A simple command line tool for zapping RFI

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  bool display = true;
  bool write = false;

  vector<string> archives;
  
  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVDw")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for zapping RFI in Pulsar::Archives"  << endl;
      cout << "Usage: paz [options] filenames"                 << endl;
      cout << "  -v   Verbose mode"                            << endl;
      cout << "  -V   Very verbose mode"                       << endl;
      cout << "  -D   Display results"                         << endl;
      cout << "  -w   Write changes to the file"               << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'D':
      display = true;
      break;
    case 'w':
      write = true;
      break;
    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    exit(-1);
  } 
  
  Pulsar::Plotter plotter;
  
  if (display) {
    cpgopen("/xs");
    cpgask(1);
  }
  
  // Instantiate an RFIMitigation class
  
  Pulsar::RFIMitigation* zapper = new Pulsar::RFIMitigation;
  
  // Start zapping archives
  
  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      Pulsar::Archive* arch = Pulsar::Archive::load(archives[i]);
      
      cout << "Loaded archive: " << archives[i] << endl;
      
      zapper->zap_chans(arch);
      
      cout << "Zapping complete" << endl;
      
      if (display)
	plotter.bandpass(arch);
      
      if (write) {
	arch->unload();
	cout << "Archive updated on disk" << endl;
      }
    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }
  if (display)
    cpgend();
}
