#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"
#include <time.h>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

// Extensions this program understands

#include "Pulsar/ProcHistory.h"


// PAM: A command line tool for modifying archives

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

  bool defaraday = false;
  double rm = 0.0;
  
  bool reset_weights = false;
  float new_weight = 1.0;

  bool smear = false;
  float smear_dc = 0.0;

  bool rotate = false;
  double rphase = 0.0;

  bool pscr = false;

  bool invint = false;

  bool stokesify = false;

  bool flipsb = false;

  string command = "pam";

  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVime:TFpIt:f:b:d:s:r:w:D:SB")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for manipulating Pulsar::Archives"                       << endl;
      cout << "Usage: pam [options] filenames"                                    << endl;
      cout << "  -v               Verbose mode"                                   << endl;
      cout << "  -V               Very verbose mode"                              << endl;
      cout << "  -i               Show revision information"                      << endl;
      cout << "  -m               Modify the original files on disk"              << endl;
      cout << "  -e extension     Write new files with this extension"            << endl;
      cout << "  -T               Time scrunch"                                   << endl;
      cout << "  -F               Frequency scrunch"                              << endl;
      cout << "  -p               Polarisation scrunch"                           << endl;
      cout << "  -I               Transform to Invariant Interval"                << endl;
      cout << "  -S               Transform to Stokes parameters"                 << endl;
      cout << "  -B               Flip the sideband sense (DANGEROUS)"            << endl;
      cout << endl;
      cout << "The following options take integer arguments"                      << endl;
      cout << "  -t tscr          Time scrunch by this factor"                    << endl;
      cout << "  -f fscr          Frequency scrunch by this factor"               << endl;
      cout << "  -b bscr          Bin scrunch by this factor"                     << endl;
      cout << endl;
      cout << "The following options take floating point arguments"               << endl;
      cout << "  -d dm            Alter the header dispersion measure"            << endl;
      cout << "  -D rm            Correct for ISM faraday rotation"               << endl;
      cout << "  -s dc            Smear with this duty cycle"                     << endl;
      cout << "  -r phase         Rotate profiles by this many turns"             << endl;
      cout << "  -w weight        Reset profile weights to this value"            << endl;
      cout << endl;
      cout << "See http://astronomy.swin.edu.au/pulsar/software/manuals/pam.html" << endl;
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
      cout << "$Id: pam.C,v 1.20 2003/09/30 08:33:18 ahotan Exp $" << endl;
      return 0;
    case 'm':
      save = true;
      break;
    case 'e':
      save = true;
      ext = optarg;
      break;
    case 'T':
      tscr = true;
      command += " -T";
      break;
    case 'F':
      fscr = true;
      command += " -F";
      break;
    case 'p':
      pscr = true;
      command += " -p";
      break;
    case 'I':
      invint = true;
      pscr = false;
      command += " -I";
      break;
    case 'f':
      fscr = true;
      if (sscanf(optarg, "%d", &fscr_fac) != 1) {
	cout << "That is not a valid fscrunch factor" << endl;
	return -1;
      }
      command += " -f ";
      command += optarg;
      break;
    case 't':
      tscr = true;
      if (sscanf(optarg, "%d", &tscr_fac) != 1) {
	cout << "That is not a valid tscrunch factor" << endl;
	return -1;
      }
      command += " -t ";
      command += optarg;
      break;
    case 'b':
      bscr = true;
      if (sscanf(optarg, "%d", &bscr_fac) != 1) {
	cout << "That is not a valid bscrunch factor" << endl;
	return -1;
      }
      if (bscr_fac <= 0) {
	cout << "That is not a valid bscrunch factor" << endl;
	return -1;
      }
      command += " -b ";
      command += optarg;
      break;
    case 'd':
      dedisperse = true;
      if (sscanf(optarg, "%lf", &dm) != 1) {
	cout << "That is not a valid dispersion measure" << endl;
	return -1;
      }
      command += " -d ";
      command += optarg;
      break;
    case 'D':
      defaraday = true;
      if (sscanf(optarg, "%lf", &rm) != 1) {
	cout << "That is not a valid rotation measure" << endl;
	return -1;
      }
      command += " -D ";
      command += optarg;
      break;
    case 's':
      smear = true;
      if (sscanf(optarg, "%f", &smear_dc) != 1) {
	cout << "That is not a valid smearing duty cycle" << endl;
	return -1;
      }
      command += " -s ";
      command += optarg;
      break;
    case 'r':
      rotate = true;
      if (sscanf(optarg, "%lf", &rphase) != 1) {
	cout << "That is not a valid rotation phase" << endl;
	return -1;
      }
      if (rphase <= 0.0 || rphase >= 1.0) {
	cout << "That is not a valid rotation phase" << endl;
	return -1;
      }
      command += " -r ";
      command += optarg;
      break;
    case 'w':
      reset_weights = true;
      if (sscanf(optarg, "%f", &new_weight) != 1) {
	cout << "That is not a valid weight" << endl;
	return -1;
      }
      command += " -w ";
      command += optarg;
      break;
    case 'S':
      stokesify = true;
      break;
    case 'B':
      flipsb = true;
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

  if (!save) {
    cout << "Changes will not be saved. Use -m or -e to write results to disk" << endl;
  }

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);

      if (flipsb) {
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  vector<float> labels;
	  labels.resize(arch->get_nchan());
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    labels[j] = arch->get_Integration(i)->get_frequency(j);
	  }
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    arch->get_Integration(i)->set_frequency(j,labels[labels.size()-1-j]);
	  }
	}
	arch->set_bandwidth(-1.0 * arch->get_bandwidth());
      }

      if (reset_weights) {
	arch->uniform_weight(new_weight);
	if (verbose)
	  cout << "All profile weights set to " << new_weight << endl;
      }
      
      if (rotate) {
	double period = arch->get_Integration(0)->get_folding_period();
	arch->rotate(period*rphase);
      }
      
      if (dedisperse) {
	for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
	  arch->get_Integration(isub)->set_dispersion_measure (dm);
	
	arch->set_dispersion_measure(dm);
	if (verbose)
	  cout << "Archive dispersion measure set to " << dm << endl;
      }

      if (defaraday) {
	arch->defaraday(rm, 0.0);
	if (verbose)
	  cout << "Archive corrected for a RM of " << rm << endl;
      }

      if (stokesify) {
	if (arch->get_npol() != 4)
	  throw Error(InvalidState, "Convert to Stokes",
		      "Not enough polarisation information");
	arch->convert_state(Signal::Stokes);
	if (verbose)
	  cout << "Archive converted to Stokes parameters" << endl;
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
      
      if (pscr) {
	arch->pscrunch();
	if (verbose)
	  cout << arch->get_filename() << " pscrunched" << endl;
      } 

      if (invint) {
	arch->invint();
	if (verbose)
	  cout << arch->get_filename() << " invinted" << endl;
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
	
	// See if the archive contains a history that should be updated:
	
	Pulsar::ProcHistory* fitsext = arch->get<Pulsar::ProcHistory>();
	
	if (fitsext) {
	  
	  if (command.length() > 80) {
	    cout << "WARNING: ProcHistory command string truncated to 80 chars" << endl;
	    fitsext->set_command_str(command.substr(0, 80));
	  }
	  else {
	    fitsext->set_command_str(command);
	  }
	  
	}
	
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



