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
  bool display = false;
  bool write = false;
  
  vector<string> archives;
  
  bool manual_zap = false;
  vector<int> chans_to_zap;
  bool simple = false;
  
  int placeholder;
  
  int first;
  int last;
  
  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";
  
  while ((gotc = getopt(argc, argv, "hvVDwz:Z:d")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for zapping RFI in Pulsar::Archives"    << endl;
      cout << "Usage: paz [options] filenames"                   << endl;
      cout << "  -v               Verbose mode"                  << endl;
      cout << "  -V               Very verbose mode"             << endl;
      cout << "  -D               Display results"               << endl;
      cout << "  -w               Write changes to the file"     << endl;
      cout << "  -z \"chanlist\"  Zap these particular channels" << endl;
      cout << "  -Z \"a b\"        Zap chans between a and b"     << endl;
      cout << "  -d               Simple mean offset rejection"  << endl;
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
    case 'z':
      key = strtok (optarg, whitespace);
      manual_zap = true;
      while (key) {
	if (sscanf(key, "%d", &placeholder) == 1) {
	  chans_to_zap.push_back(placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      cerr << chans_to_zap.size() << endl;
      break;
    case 'Z':
      manual_zap = true;
      if (sscanf(optarg, "%d %d", &first, &last) != 2) {
	cerr << "Invalid parameter to option -Z" << endl;
        return (-1);
      }
      break;
    case 'd':
      simple = true;
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
      
      int nchan = arch->get_nchan();
      
      if (simple) {
	cout << "Using simple mean offset zapper" << endl;
	zapper->zap_chans(arch);
	cout << "Zapping complete" << endl;
      }
      else if (manual_zap) {
	if (chans_to_zap.empty()) {
	  vector<float> mask(nchan, 1.0);
	  if ((last > nchan) || (first > last) || (first < 0)) {
	    throw Error(InvalidParam, "Specified channels lie outside known range");
	  }
	  else {
	    for (int i = first; i <= last; i++) {
	      mask[i] = 0.0;
	    }
	  }
	  zapper->zap_specific(arch, mask);
	}
	else {
	  vector<float> mask(nchan, 1.0);
	  for (unsigned i = 0; i < chans_to_zap.size(); i++) {
	    mask[chans_to_zap[i]] = 0.0;
	  }
	  zapper->zap_specific(arch, mask);
	}
      }
      else {
	cout << "No zapping scheme specified!" << endl;
	return (-1);
      }
      
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


