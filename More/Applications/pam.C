#include <algorithm>

#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"
#include <time.h>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/getopt.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"

#include "Error.h"

// Extensions this program understands

#include "Pulsar/ProcHistory.h"

void usage()
{
  cout << "A program for manipulating Pulsar::Archives"                       << endl;
  cout << "Usage: pam [options] filenames"                                    << endl;
  cout << "  -v               Verbose mode"                                   << endl;
  cout << "  -V               Very verbose mode"                              << endl;
  cout << "  -i               Show revision information"                      << endl;
  cout << "  -m               Modify the original files on disk"              << endl;
  cout << "  -a archive       Write new files using this archive class"       << endl;
  cout << "  -e extension     Write new files with this extension"            << endl;
  cout << "  -u path          Write files to this location"                   << endl;
  cout << "  -T               Time scrunch to one subint"                     << endl;
  cout << "  -F               Frequency scrunch to one channel"               << endl;
  cout << "  -p               Polarisation scrunch to total intensity"        << endl;
  cout << "  -I               Transform to Invariant Interval"                << endl;
  cout << "  -S               Transform to Stokes parameters"                 << endl;
  cout << "  -x \"start end\"   Extract subints in this inclusive range"      << endl;
  cout << endl;
  cout << "The following options take integer arguments"                      << endl;
  cout << "  -t               Time scrunch by this factor"                    << endl;
  cout << "  -f               Frequency scrunch by this factor"               << endl;
  cout << "  -b               Bin scrunch by this factor"                     << endl;
  cout << "  --setnsub        Time scrunch to this many subints"              << endl;
  cout << "  --setnchn        Frequency scrunch to this many channels"        << endl;
  cout << "  --setnbin        Bin scrunch to this many bins"                  << endl;
  cout << "  --binphsperi     Convert to binary phase periastron order"       << endl;
  cout << "  --binphsasc      Convert to binary phase asc node order"         << endl;
  cout << "  --binlngperi     Convert to binary longitude periastron order"   << endl;
  cout << "  --binlngasc      Convert to binary longitude asc node order"     << endl;
  cout << endl;
  cout << "The following options take floating point arguments"               << endl;
  cout << "  -d               Alter the header dispersion measure"            << endl;
  cout << "  -D               Correct for ISM faraday rotation"               << endl;
  cout << "  -s               Smear with this duty cycle"                     << endl;
  cout << "  -r               Rotate profiles by this many turns"             << endl;
  cout << "  -w               Reset profile weights to this value"            << endl;
  cout << endl;
  cout << "The following options override archive paramaters"                 << endl;
  cout << "  -L               Set feed basis to Linear"                       << endl;
  cout << "  -C               Set feed basis to Circular"                     << endl;
  cout << "  -E ephfile       Install a new ephemeris and update model"       << endl;
  cout << "  -B               Flip the sideband sense"                        << endl;
  cout << "  -o centre_freq   Change the frequency labels"                    << endl;
  cout << endl;
  cout << "See http://astronomy.swin.edu.au/pulsar/software/manuals/pam.html" << endl;
  return;
}

// PAM: A command line tool for modifying archives

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  
  vector<string> archives;

  string ulpath;

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

  bool new_eph = false;
  string eph_file;

  string command = "pam";

  char* archive_class = 0;

  int new_nchn = 0;
  int new_nsub = 0;
  int new_nbin = 0;

  bool circ = false;
  bool lin = false;

  unsigned ronsub = 0;
  bool cbppo = false;
  bool cbpao = false;
  bool cblpo = false;
  bool cblao = false;

  int subint_extract_start = -1;
  int subint_extract_end = -1;

  bool new_cfreq = false;
  float new_fr = 0.0;

  Reference::To<Pulsar::IntegrationOrder> myio = 0;

  int c = 0;
  
  while (1) {

    int options_index = 0;

    static struct option long_options[] = {
      {"setnchn",    1, 0, 200},
      {"setnsub",    1, 0, 201},
      {"setnbin",    1, 0, 202},
      {"binphsperi", 1, 0, 203},
      {"binphsasc",  1, 0, 204},
      {"binlngperi", 1, 0, 205},
      {"binlngasc",  1, 0, 206},
      {0, 0, 0, 0}
    };
    
    c = getopt_long(argc, argv, "hvVima:e:E:TFpIt:f:b:d:o:s:r:u:w:D:SBLCx:",
		    long_options, &options_index);
    
    if (c == -1)
      break;

    switch (c) {
    case 'h':
      usage();
      return (0);
      break;
    case 'v':
      verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;
    case 'i':
      cout << "$Id: pam.C,v 1.32 2004/03/10 00:04:18 ahotan Exp $" << endl;
      return 0;
    case 'm':
      save = true;
      break;
    case 'L':
      lin = true;
      break;
    case 'C':
      circ = true;
      break;
    case 'a':
      archive_class = optarg;
      break;
    case 'e':
      save = true;
      ext = optarg;
      break;
    case 'E':
      new_eph = true;
      eph_file = optarg;
      command += " -E";
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
    case 'o':
      new_cfreq = true;
      if (sscanf(optarg, "%f", &new_fr) != 1) {
	cout << "That is not a valid centre frequency" << endl;
	return -1;
      }
      command += " -o ";
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
    case 'u':
      ulpath = optarg;
      if (ulpath.substr(ulpath.length()-1,1) != "/")
	ulpath += "/";
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
    case 'x' :
      sscanf(optarg,"%d %d",&subint_extract_start,&subint_extract_end);
      subint_extract_end++;
      break;
    case 200:
      fscr = true;
      if (sscanf(optarg, "%d", &new_nchn) != 1) {
	cout << "That is not a valid number of channels" << endl;
	return -1;
      }
      if (new_nchn <= 0) {
	cout << "That is not a valid number of channels" << endl;
	return -1;
      }
      command += " --setnchn ";
      command += optarg;
      break;
    case 201:
      tscr = true;
      if (sscanf(optarg, "%d", &new_nsub) != 1) {
	cout << "That is not a valid number of subints" << endl;
	return -1;
      }
      if (new_nsub <= 0) {
	cout << "That is not a valid number of subints" << endl;
	return -1;
      }
      command += " --setnsub ";
      command += optarg;
      break;
    case 202:
      bscr = true;
      if (sscanf(optarg, "%d", &new_nbin) != 1) {
	cout << "That is not a valid number of bins" << endl;
	return -1;
      }
      if (new_nbin <= 0) {
	cout << "That is not a valid number of bins" << endl;
	return -1;
      }
      command += " --setnbin ";
      command += optarg;
      break;
    case 203: {
      if (cbpao || cblpo || cblao) {
	cerr << "You can only specify one re-ordering scheme!"
	     << endl;
	return -1;
      }
      if (sscanf(optarg, "%ud", &ronsub) != 1) {
	cerr << "Invalid nsub given" << endl;
	return -1;
      }
      cbppo = true;
      break;
    } 
    case 204: {
      if (cbppo || cblpo || cblao) {
	cerr << "You can only specify one re-ordering scheme!"
	     << endl;
	return -1;
      }
      if (sscanf(optarg, "%ud", &ronsub) != 1) {
	cerr << "Invalid nsub given" << endl;
	return -1;
      }
      cbpao = true;
      break;
    }      
    case 205: {
      if (cblao || cbppo || cbpao) {
	cerr << "You can only specify one re-ordering scheme!"
	     << endl;
	return -1;
      }
      if (sscanf(optarg, "%ud", &ronsub) != 1) {
	cerr << "Invalid nsub given" << endl;
	return -1;
      }
      cblpo = true;
      break;
    } 
    case 206: {
      if (cblpo || cbppo || cbpao) {
	cerr << "You can only specify one re-ordering scheme!"
	     << endl;
	return -1;
      }
      if (sscanf(optarg, "%ud", &ronsub) != 1) {
	cerr << "Invalid nsub given" << endl;
	return -1;
      }
      cblao = true;
      break;
    }
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

      if (lin) {
	arch->set_basis(Signal::Linear);
	cout << "Feed basis set to Linear" << endl;
      }

      if (circ) {
	arch->set_basis(Signal::Circular);
	cout << "Feed basis set to Circular" << endl;
      }

      if (new_cfreq) {
	float nc = arch->get_nchan();
	float bw = arch->get_bandwidth();
        float cw = bw / nc;

	float fr = new_fr - (bw / 2.0) + (cw / 2.0);
	
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    arch->get_Integration(i)->set_frequency(j,(fr + (j*cw)));
	  }
	}
	
	arch->set_centre_frequency(new_fr);
      }

      if (new_eph) {
	if (!eph_file.empty()) {
	  psrephem eph;
	  if (eph.load(eph_file) == 0)
	    arch->set_ephemeris(eph);
	  else
	    cerr << "Could not load new ephemeris" << endl;
	}
      }

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

      if (cbppo) {
	myio = new Pulsar::PeriastronOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cbpao) {
	myio = new Pulsar::BinaryPhaseOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cblpo) {
	myio = new Pulsar::BinLngPeriOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if (cblao) {
	myio = new Pulsar::BinLngAscOrder();
	arch->add_extension(myio);
	myio->organise(arch, ronsub);
      }
      
      if( subint_extract_start >= 0 && subint_extract_end >= 0 ) {
	vector<unsigned> subints;
	unsigned isub = subint_extract_start;

	while ( isub < arch->get_nsubint() && isub < unsigned(subint_extract_end) ){
	  subints.push_back( isub );
	  isub++;
	}

	Reference::To<Pulsar::Archive> extracted( arch->extract(subints) );
	extracted->set_filename( arch->get_filename() );

	arch = extracted;
      }

      if (tscr) {
	if (new_nsub > 0) {
	  arch->tscrunch_to_nsub(new_nsub);
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched to " 
		 << new_nsub << " subints" << endl;
	}
	else if (tscr_fac > 0) {
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
	if (new_nchn > 0) {
	  arch->fscrunch_to_nchan(new_nchn);
	  if (verbose)
	    cout << arch->get_filename() << " fscrunched to " 
		 << new_nchn << " channels" << endl;
	}
	else if (fscr_fac > 0) {
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
	if (new_nbin > 0) {
	  arch->bscrunch_to_nbin(new_nbin);
	  if (verbose)
	    cout << arch->get_filename() << " bscrunched to " 
		 << new_nbin << " bins" << endl;
	}
	else {
	  arch->bscrunch(bscr_fac);
	  if (verbose)
	    cout << arch->get_filename() << " bscrunched by a factor of " 
		 << bscr_fac << endl;
	}
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

        if (archive_class)  {

          // unload an archive of the specified class
          Reference::To<Pulsar::Archive> output;
          output = Pulsar::Archive::new_Archive (archive_class);
          output -> copy (*arch);
          output -> set_filename ( arch->get_filename() );

          arch = output;

        }


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
	  string the_new;
	  if (!ulpath.empty())
	    the_new = ulpath + primary + "." + ext;
	  else
	    the_new = primary + "." + ext;
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



