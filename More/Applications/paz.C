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
  bool write = true;
  
  vector<string> archives;
  
  bool manual_zap = false;
  vector<int> chans_to_zap;

  bool edge_zap = false;
  float percent = 0.0;
  
  bool simple = false;
  
  string ext;

  int placeholder;
  
  int first;
  int last;
  
  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";
  
  while ((gotc = getopt(argc, argv, "hvVDxe:z:Z:dE:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for zapping RFI in Pulsar::Archives"               << endl;
      cout << "Usage: paz [options] filenames"                              << endl;
      cout << "  -v               Verbose mode"                             << endl;
      cout << "  -V               Very verbose mode"                        << endl;
      cout << "  -D               Display results"                          << endl;
      cout << "  -x               Test only, leaves files unchanged"        << endl;
      cout << "  -e               Unload to new files using this extension" << endl;
      cout << "  -z \"chanlist\"    Zap these particular channels"          << endl;
      cout << "  -Z \"a b\"         Zap chans between a and b"              << endl;
      cout << "  -E percent       Zap band edges"                           << endl; 
      cout << "  -d               Simple mean offset rejection"             << endl;
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
    case 'x':
      write = false;
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
    case 'e':
      ext = optarg;
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
    case 'E':
      edge_zap = true;
      if (sscanf(optarg, "%f", &percent) != 1) {
	cerr << "Invalid parameter to option -e" << endl;
        return (-1);
      }
      if (percent <= 0.0 || percent >= 100.0) {
	cerr << "Invalid parameter to option -e" << endl;
        return (-1);
      }
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
      else if (edge_zap) {
	float fraction = percent / 100.0;
	int buffer = int(float(nchan) * fraction);
	
	vector<float> mask(nchan, 0.0);
	
	for (int i = (0 + buffer); i <= (nchan - buffer); i++) {
	  mask[i] = 1.0;
	}
	zapper->zap_specific(arch, mask);
      }
      else {
	cout << "No zapping scheme specified!" << endl;
	return (-1);
      }
      
      if (display)
	plotter.bandpass(arch);
      
      if (write) {
	if (ext.empty()) {
	  arch->unload();
	  cout << "Archive " << arch->get_filename() << " updated on disk" << endl;
	}
	else {
	  string the_old = arch->get_filename();
	  int index = the_old.find_last_of(".",the_old.length());
	  string primary = the_old.substr(0, index);
	  string the_new = primary + "." + ext;
	  arch->unload(the_new);
	  cout << "New file " << the_new << " written to disk" << endl;
	}
      }
    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }
  if (display)
    cpgend();
}


