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
  string killfile;
  vector<int> chans_to_zap;

  bool zap_subints = false;
  bool zero_subints = false;
  vector<unsigned> subs_to_zap;

  bool edge_zap = false;
  float percent = 0.0;
  
  bool simple = false;
  
  bool zap_ston = false;
  double ston_cutoff = 0.0;
  
  bool std_given = false;
  Reference::To<Pulsar::Profile> thestd;
  
  string ext;
  
  int placeholder;
  
  int first;
  int last;
  
  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";
  
  while ((gotc = getopt(argc, argv, "hvViDme:z:k:Z:dE:s:w:S:P:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for zapping RFI in Pulsar::Archives"                     << endl;
      cout << "Usage: paz [options] filenames"                                    << endl;
      cout << "  -v               Verbose mode"                                   << endl;
      cout << "  -V               Very verbose mode"                              << endl;
      cout << "  -i               Show revision information"                      << endl;
      cout << "  -D               Display resulting bandpass and weights"         << endl;
      cout << "  -m               Modify the original files on disk"              << endl;
      cout << "  -e               Unload to new files using this extension"       << endl;
      cout << "  -z \"a b c ...\"   Zap these particular channels"                << endl;
      cout << "  -k filename      Zap chans listed in this kill file"             << endl;
      cout << "  -Z \"a b\"         Zap chans between a and b"                    << endl;
      cout << "  -E percent       Zap this much of the band at the edges"         << endl;
      cout << "  -s \"a b c ...\"   Delete these sub-integrations"                << endl;
      cout << "  -w \"a b c ...\"   Zap (zero weight) these sub-integrations"     << endl;
      cout << "  -d               Use simple mean offset spike zapping"           << endl;
      cout << "  -S cutoff        Zap channels based on S/N (using std if given)" << endl;
      cout << "  -P stdfile       Use this standard profile"                      << endl;
      cout << endl;
      cout << "The format of the kill file used with the -k option is simply"    << endl;
      cout << "a list of channel numbers, separated by spaces or newlines"  << endl;
      cout << endl;
      cout << "The cutoff S/N value used with -S is largely arbitrary. You will" << endl;
      cout << "need to experiment to find the best value for your archives" << endl;
      cout << endl;
      cout << "See http://astronomy.swin.edu.au/pulsar/software/manuals/paz.html" << endl;
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
      cout << "$Id: paz.C,v 1.14 2003/11/07 04:03:50 ahotan Exp $" << endl;
      return 0;
    case 'D':
      display = true;
      break;
    case 'm':
      write = true;
      break;
    case 'k':
      killfile = optarg;
      manual_zap = true;
    case 'z':
      key = strtok (optarg, whitespace);
      manual_zap = true;
      while (key) {
	if (sscanf(key, "%d", &placeholder) == 1) {
	  chans_to_zap.push_back(placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;
    case 'e':
      write = true;
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
    case 's':
      key = strtok (optarg, whitespace);
      zap_subints = true;
      while (key) {
	if (sscanf(key, "%d", &placeholder) == 1) {
	  subs_to_zap.push_back(placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;
    case 'w':
      key = strtok (optarg, whitespace);
      zero_subints = true;
      while (key) {
	if (sscanf(key, "%d", &placeholder) == 1) {
	  subs_to_zap.push_back(placeholder);
	}
	key = strtok (NULL, whitespace);
      }
      break;
    case 'S':
      zap_ston = true;
      if (sscanf(optarg, "%lf", &ston_cutoff) != 1) {
	cerr << "Invalid parameter to option -S" << endl;
        return (-1);
      }
      break;
    case 'P':
      try {
	Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(optarg);
	data->pscrunch();
	data->fscrunch();
	data->tscrunch();
	thestd = data.release()->get_Profile(0,0,0);
	std_given = true;
      }
      catch (Error& error) {
	cout << error << endl;
	cout << "Could not load given standard profile" << endl;
	std_given = false;
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
  
  if (!write)
    cout << "Changes will not be saved. Use -m or -e to write results to disk" << endl;
  
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
      
      Reference::To<Pulsar::Archive> arch = Pulsar::Archive::load(archives[i]);
      
      cout << "Loaded archive: " << archives[i] << endl;
      
      int nchan = arch->get_nchan();

      if (zap_subints) {

	Reference::To<Pulsar::Archive> new_arch;

	vector<unsigned> subs_to_keep;
	bool ignore;
	
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  ignore = false;
	  for (unsigned j = 0; j < subs_to_zap.size(); j++)
	    if (subs_to_zap[j] == i) {
	      ignore = true;
	      if (verbose)
		cout << "Zapping subint " << i << endl;
	    }
	  if (!ignore) {
	    subs_to_keep.push_back(i);
	  }
	}
	new_arch = arch->extract(subs_to_keep);
	string useful = arch->get_filename();
	arch = new_arch->clone();
	arch->set_filename(useful);
      }
      
      if (zero_subints) {
	vector<float> mask(nchan, 0.0);
	zapper->zap_very_specific(arch,mask,subs_to_zap);
      }
      
      if (simple) {
	cout << "Using simple mean offset zapper" << endl;
	zapper->zap_chans(arch);
	cout << "Zapping complete" << endl;
      }
      else if (manual_zap) {
	if (!killfile.empty()) {
	  vector<int> some_chans;
	  char* useful = new char[4096];
	  FILE* fptr = fopen(killfile.c_str(), "r");
	  while (fgets(useful, 4096, fptr)) {
	    key = strtok (useful, whitespace);
	    while (key) {
	      if (sscanf(key, "%d", &placeholder) == 1) {
		some_chans.push_back(placeholder);
	      }
	      key = strtok (NULL, whitespace);
	    }
	    for (unsigned x = 0; x < some_chans.size(); x++)
	      chans_to_zap.push_back(some_chans[x]);
	    some_chans.resize(0);
	  }
	  delete[] useful;
	}
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
      else if (zap_ston) {
	double theston = 0.0;
	arch->pscrunch();
	for (unsigned isub = 0; isub < arch->get_nsubint(); isub++) {
	  for (unsigned ichan = 0; ichan < arch->get_nchan(); ichan++) {
	    theston = arch->get_Profile(isub,0,ichan)->snr();
	    if (theston < ston_cutoff) {
	      arch->get_Integration(isub)->set_weight(ichan, 0.0);
	    } 
	  }
	}
      }
      else if (zap_ston && thestd) {
	double theston = 0.0;
	arch->pscrunch();
	for (unsigned isub = 0; isub < arch->get_nsubint(); isub++) {
	  for (unsigned ichan = 0; ichan < arch->get_nchan(); ichan++) {
	    theston = arch->get_Profile(isub,0,ichan)->snr(thestd);
	    if (theston < ston_cutoff) {
	      arch->get_Integration(isub)->set_weight(ichan, 0.0);
	    } 
	  }
	}
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


