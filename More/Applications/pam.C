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
  
  bool update = true;
  string ext;
  
  bool tscr = false;
  int tscr_fac = 0;

  bool fscr = false;
  int fscr_fac = 0;

  bool bscr = false;
  int bscr_fac = 0;

  bool smear = false;
  float smear_dc = 0.0;

  bool pscr = false;

  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVxe:TFpt:f:b:s:")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for manipulating Pulsar::Archives"            << endl;
      cout << "Usage: pam [options] filenames"                         << endl;
      cout << "  -v               Verbose mode"                        << endl;
      cout << "  -V               Very verbose mode"                   << endl;
      cout << "  -x               Test only, files are not changed"    << endl;
      cout << "  -e [string]      Write new files with this extension" << endl;
      cout << endl;
      cout << "  -T               Time scrunch"                        << endl;
      cout << "  -F               Frequency scrunch"                   << endl;
      cout << "  -p               Polarisation scrunch"                << endl;
      cout << "  -t [int]         Time scrunch by this factor"         << endl;
      cout << "  -f [int]         Frequency scrunch by this factor"    << endl;
      cout << "  -b [int]         Bin scrunch by this factor"          << endl;
      cout << "  -s [float]       Smear with this duty cycle"          << endl;
      return (-1);
      break;
    case 'v':
      verbose = true;
      Error::verbose = true;
      break;
    case 'V':
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'x':
      update = false;
      break;
    case 'e':
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
    case 's':
      smear = true;
      if (sscanf(optarg, "%f", &smear_dc) != 1) {
	cout << "That is not a valid smearing duty cycle." << endl;
	return -1;
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
  
  Reference::To<Pulsar::Archive> arch;

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);
      
      if (tscr) {
	if (tscr_fac > 0)
	  arch->tscrunch(tscr_fac);
	else
	  arch->tscrunch();
      }
      
      if (fscr) {
	if (fscr_fac > 0)
	  arch->fscrunch(fscr_fac);
	else
	  arch->fscrunch();
      }
      
      if (bscr)
	arch->bscrunch(bscr_fac);
      
      if (pscr)
	arch->pscrunch();
      
      if (smear) {
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_npol(); j++) {
	    for (unsigned k = 0; k < arch->get_nchan(); k++) {
	      arch->get_Profile(i,j,k)->smear(smear_dc);
	    }
	  }
	}
      }
      
      if (update) {
	if (ext.empty()) {
	  arch->unload();
	  cout << "Archive updated on disk" << endl;
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



