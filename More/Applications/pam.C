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

  bool reset_weights = false;
  float new_weight = 1.0;

  bool smear = false;
  float smear_dc = 0.0;

  bool rotate = false;
  double rphase = 0.0;

  bool pscr = false;

  bool invint = false;

  string command = "pam";

  int gotc = 0;
  
  while ((gotc = getopt(argc, argv, "hvVme:TFpit:f:b:d:s:r:w:")) != -1) {
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
      cout << "  -i               Transform to Invariant Interval"     << endl;
      cout << "  -t [int f]       Time scrunch by a factor of f"       << endl;
      cout << "  -f [int f]       Frequency scrunch by a factor of f"  << endl;
      cout << "  -b [int f]       Bin scrunch by a factor of f"        << endl;
      cout << "  -d [float dm]    Dedisperse to dm"                    << endl;
      cout << "  -s [float c]     Smear with duty cycle c"             << endl;
      cout << "  -r [float p]     Rotate profiles by phase p"          << endl;
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
    case 'i':
      invint = true;
      pscr = false;
      command += " -i";
      break;
    case 'f':
      fscr = true;
      if (sscanf(optarg, "%d", &fscr_fac) != 1) {
	cout << "That is not a valid fscrunch factor." << endl;
	return -1;
      }
      command += " -f ";
      command += optarg;
      break;
    case 't':
      tscr = true;
      if (sscanf(optarg, "%d", &tscr_fac) != 1) {
	cout << "That is not a valid tscrunch factor." << endl;
	return -1;
      }
      command += " -t ";
      command += optarg;
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
      command += " -b ";
      command += optarg;
      break;
    case 'd':
      dedisperse = true;
      if (sscanf(optarg, "%lf", &dm) != 1) {
	cout << "That is not a valid dispersion measure." << endl;
	return -1;
      }
      command += " -d ";
      command += optarg;
      break;
    case 's':
      smear = true;
      if (sscanf(optarg, "%f", &smear_dc) != 1) {
	cout << "That is not a valid smearing duty cycle." << endl;
	return -1;
      }
      command += " -s ";
      command += optarg;
      break;
    case 'r':
      rotate = true;
      if (sscanf(optarg, "%lf", &rphase) != 1) {
	cout << "That is not a valid rotation phase." << endl;
	return -1;
      }
      if (rphase <= 0.0 || rphase >= 1.0) {
	cout << "That is not a valid rotation phase." << endl;
	return -1;
      }
      command += " -r ";
      command += optarg;
      break;
    case 'w':
      reset_weights = true;
      if (sscanf(optarg, "%f", &new_weight) != 1) {
	cout << "That is not a valid weight." << endl;
	return -1;
      }
      command += " -w ";
      command += optarg;
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
      
      if (rotate) {
	double period = arch->get_Integration(0)->get_folding_period();
	arch->rotate(period*rphase);
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
	
	Pulsar::ProcHistory* fitsext = 0;
	for (unsigned i = 0; i < arch->get_nextension(); i++) {
	  Pulsar::Archive::Extension* extension;
	  extension = (Pulsar::Archive::Extension*)arch->get_extension (i);
	  fitsext = dynamic_cast<Pulsar::ProcHistory*> (extension);
	  if (fitsext) {
	    break;
	  }
	}
	
	if (fitsext) {
	  
	  fitsext->add_blank_row();
	  
	  if (command.length() > 80) {
	    cout << "WARNING: ProcHistory command string truncated to 80 chars" << endl;
	    fitsext->get_last().proc_cmd = command.substr(0, 80);
	  }
	  else {
	    fitsext->get_last().proc_cmd = command;
	  }
	  
	  time_t myt;
	  time(&myt);
	  fitsext->get_last().date_pro = ctime(&myt);
	  
	  fitsext->get_last().pol_type = state_string(arch->get_state());
	  
	  fitsext->get_last().npol = arch->get_npol();
	  fitsext->get_last().nbin = arch->get_nbin();
	  fitsext->get_last().nbin_prd = arch->get_nbin();
	  fitsext->get_last().tbin = ((arch->get_Integration(0)->get_folding_period())/arch->get_nbin());
	  fitsext->get_last().ctr_freq = arch->get_centre_frequency();
	  fitsext->get_last().nchan = arch->get_nchan();
	  fitsext->get_last().chanbw = (arch->get_bandwidth())/float(arch->get_nchan());
	  
	  if (arch->get_parallactic_corrected()) {
	    fitsext->get_last().par_corr = 1;
	  }
	  else {
	    fitsext->get_last().par_corr = 0;
	  }
	  
	  if (arch->get_iono_rm_corrected() && arch->get_ism_rm_corrected()) {
	    fitsext->get_last().rm_corr = 1;
	  }
	  else {
	    fitsext->get_last().rm_corr = 0;
	  }
	  
	  if (arch->get_dedispersed()) {
	    fitsext->get_last().dedisp = 1;
	  }
	  else {
	    fitsext->get_last().dedisp = 0;
	  }
	  
	  //sc_mthd
	  //cal_mthd
	  //cal_file
	  //rfi_mthd
	  
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



