#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

// A command line tool for modifying archives

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  
  vector<string> archives;

  bool save = false;
  string ext;
  
  bool tscr = false;
  int tscr_fac = 0;

  bool fscr = false;
  int fscr_fac = 0;

  bool bscr = false;
  int bscr_fac = 0;

  bool dedisperse = false;
  double dm = 0.0;

  bool reset_weights = false;
  float new_weight = 1.0;

  bool smear = false;
  float smear_dc = 0.0;

  bool pscr = false;

  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVme:TFpt:f:b:d:s:w:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for manipulating Pulsar::Archives"            << endl;
      cout << "Usage: pam [options] filenames"                         << endl;
      cout << "  -v               Verbose mode"                        << endl;
      cout << "  -V               Very verbose mode"                   << endl;
      cout << "  -m               Modify the original data"            << endl;
      cout << "  -e [string e]    Write new files with extension e"    << endl;
      cout << endl;
      cout << "  -T               Time scrunch"                        << endl;
      cout << "  -F               Frequency scrunch"                   << endl;
      cout << "  -p               Polarisation scrunch"                << endl;
      cout << "  -t [int f]       Time scrunch by a factor of f"       << endl;
      cout << "  -f [int f]       Frequency scrunch by a factor of f"  << endl;
      cout << "  -b [int f]       Bin scrunch by a factor of f"        << endl;
      cout << "  -d [float dm]    Dedisperse to dm"                    << endl;
      cout << "  -s [float c]     Smear with duty cycle c"             << endl;
      cout << "  -w [float w]     Reset all profile weights to w"      << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'm':
      save = true;
      break;
    case 'e':
      save = true;
      ext = optarg;
      break;
    case 'T':
      tscr = true;
      break;
    case 'F':
      fscr = true;
      break;
    case 'p':
      pscr = true;
      break;
    case 'f':
      fscr = true;
      if (sscanf(optarg, "%d", &fscr_fac) != 1) {
	cout << "That is not a valid fscrunch factor." << endl;
	return -1;
      }
      break;
    case 't':
      tscr = true;
      if (sscanf(optarg, "%d", &tscr_fac) != 1) {
	cout << "That is not a valid tscrunch factor." << endl;
	return -1;
      }
      break;
    case 'b':
      bscr = true;
      if (sscanf(optarg, "%d", &bscr_fac) != 1) {
	cout << "That is not a valid bscrunch factor." << endl;
	return -1;
      }
      if (bscr_fac <= 0) {
	cout << "That is not a valid bscrunch factor." << endl;
	return -1;
      }
      break;
    case 'd':
      dedisperse = true;
      if (sscanf(optarg, "%lf", &dm) != 1) {
	cout << "That is not a valid dispersion measure." << endl;
	return -1;
      }
      break;
    case 's':
      smear = true;
      if (sscanf(optarg, "%f", &smear_dc) != 1) {
	cout << "That is not a valid smearing duty cycle." << endl;
	return -1;
      }
      break;
    case 'w':
      reset_weights = true;
      if (sscanf(optarg, "%f", &new_weight) != 1) {
	cout << "That is not a valid weight." << endl;
	return -1;
      }
      break;
    default:
      cout << "Unrecognised option." << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    exit(-1);
  } 
  
  Reference::To<Pulsar::Archive> arch;

  if (!save) {
    cout << "Changes will not be saved. Use -m or -e to write results to disk." << endl;
  }

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);

      if (reset_weights) {
	arch->uniform_weight(new_weight);
	if (verbose)
	  cout << "All profile weights set to " << new_weight << endl;
      }

      if (dedisperse) {
	arch->dedisperse(dm,arch->get_centre_frequency());
	if (verbose)
	  cout << "Archive dedispersed to a DM of " << dm << endl;
      }
      
      if (tscr) {
	if (tscr_fac > 0) {
	  arch->tscrunch(tscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched by a factor of " 
		 << tscr_fac << endl;
	}
	else {
	  arch->tscrunch();
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched" << endl;
	}
      }
      
      if (fscr) {
	if (fscr_fac > 0) {
	  arch->fscrunch(fscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched by a factor of " 
		 << fscr_fac << endl;
	}
	else {
	  arch->fscrunch();
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched" << endl;
	}
      }
      
      if (bscr) {
	arch->bscrunch(bscr_fac);
	if (verbose)
	  cout << arch->get_filename() << " bscrunched by a factor of " 
	       << bscr_fac << endl;
      }
      
      if (pscr) {
	arch->pscrunch();
	if (verbose)
	  cout << arch->get_filename() << " pscrunched" << endl;
      }      

      if (smear) {
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_npol(); j++) {
	    for (unsigned k = 0; k < arch->get_nchan(); k++) {
	      arch->get_Profile(i,j,k)->smear(smear_dc);
	    }
	  }
	}
      }
      
      if (save) {
	if (ext.empty()) {
	  arch->unload();
	  cout << arch->get_filename() << " updated on disk" << endl;
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

}



