/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/psrchive.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

// Extensions this program understands

#include "Pulsar/ProcHistory.h"
#include "Pulsar/Backend.h"
#include "Pulsar/BackendName.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/FaradayRotation.h"

#include "dirutil.h"
#include "genutil.h"
#include "string_utils.h"
#include "Error.h"


#include "getopt.h"
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <new>

#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>


void usage()
{
  cout << "A program for manipulating Pulsar::Archives \n"
    "Usage: pam [options] filenames \n"
    "  -q               Quiet mode \n"
    "  -v               Verbose mode \n"
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "  -m               Modify the original files on disk \n"
    "  -M metafile      List of archive filenames in metafile \n"
    "  -a archive       Write new files using this archive class \n"
    "  -e extension     Write new files with this extension \n"
    "  -u path          Write files to this location \n"
    "  -T               Time scrunch to one subint \n"
    "  -F               Frequency scrunch to one channel \n"
    "  -p               Polarisation scrunch to total intensity \n"
    "  -D               Dedisperse (but not fscrunch) \n"
    "  --DD             Dededisperse (i.e. undo -D option) \n"
    "  -I               Transform to Invariant Interval \n"
    "  -S               Transform to Stokes parameters \n"
    "  -x \"start end\"   Extract subints in this inclusive range \n"
    "\n"
    "The following options take integer arguments \n"
    "  -t               Time scrunch by this factor \n"
    "  -f               Frequency scrunch by this factor \n"
    "  -b               Bin scrunch by this factor \n"
    "  --setnsub        Time scrunch to this many subints \n"
    "  --settsub        Time scrunch to this subint length \n"
    "  --setnchn        Frequency scrunch to this many channels \n"
    "  --setnbin        Bin scrunch to this many bins \n"
    "  --binphsperi     Convert to binary phase periastron order \n"
    "  --binphsasc      Convert to binary phase asc node order \n"
    "  --binlngperi     Convert to binary longitude periastron order \n"
    "  --binlngasc      Convert to binary longitude asc node order \n"
    "\n"
    "The following options take floating point arguments but --RR doesn't \n"
    "  -d DM            Alter the header dispersion measure \n"
    "  -R RM            Correct for ISM faraday rotation \n"
    "  --RR             Dedefaraday (i.e. undo -R option) \n"
    "  --RM RM          Install a new RM but don't defaraday \n"
    "  --inf            Use infinite frequency as reference when defaradaying\n"
    "  -s               Smear with this duty cycle \n"
    "  -r               Rotate profiles by this many turns \n" 
    "  -w               Reset profile weights to this value \n"
    "\n"
    "The following options override archive paramaters \n"
    "  --receiver file  Install the Receiver described in the text file \n"
    "  -L               Set feed basis to Linear \n"
    "  -C               Set feed basis to Circular \n"
    "  -E ephfile       Install a new ephemeris and update model \n"
    "  -B               Flip the sideband sense \n"
    "  --reverse_freqs  Reverse the ordering of the frequency channels and\n"
    "                   change the bandwidth flag accordingly\n"
    "  -o centre_freq   Change the frequency labels \n"
    "  --type type      Change the 'type' parameter where 'type' is one of:\n"
    "                   'Pulsar', 'PolnCal', 'FluxCalOn', 'FluxCalOff', 'Calibrator'\n"
    "  --inst inst      Change the instrument name (Archive must have\n"
    "                   'BackendName' extension for this to work)\n"
    "  --site site      Correct 'site' of telescope (One letter tempo code- GBT=1, PKS=7 etc)\n"
    "  --name name      Change source name\n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pam for more details\n"
       << endl;

  return;
}

// PAM: A command line tool for modifying archives

int main (int argc, char *argv[]) try {
  
    bool verbose = false;
    char* metafile = 0;
 
    string ulpath;

    bool save = false;
    string ext;
  
    bool tscr = false;
    int tscr_fac = 0;

    bool fscr = false;
    int fscr_fac = 0;

    bool bscr = false;
    int bscr_fac = 0;

    bool newdm = false;
    double dm = 0.0;

    bool scattered_power_correction = false;

    bool defaraday = false;
    bool dedefaraday = false;
    double rm = 9.9e99;
  
    bool reset_weights = false;
    float new_weight = 1.0;

    bool smear = false;
    float smear_dc = 0.0;

    bool rotate = false;
    double rphase = 0.0;

    bool dedisperse = false;
    bool dededisperse = false;

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

    float tsub = 0.0;

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
    Signal::Source new_type = Signal::Unknown;
    string instrument;
    bool reverse_freqs = false;
    string site;
    string name;
    bool defaraday_to_infinity = false;

    Reference::To<Pulsar::IntegrationOrder> myio;
    Reference::To<Pulsar::Receiver> install_receiver;

    int c = 0;

    const int TYPE = 1208;
    const int INST = 1209;
    const int REVERSE_FREQS = 1210;
    const int SITE = 1211;
    const int NAME = 1212;
    const int DD   = 1213;
    const int RR   = 1214;
    const int SPC  = 1215;
    const int RM   = 1216;
    const int INF  = 1217;

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
	{"receiver",   1, 0, 207},
	{"settsub",    1, 0, 208},
	{"type",       1, 0, TYPE},
	{"inst",       1, 0, INST},
	{"reverse_freqs",no_argument,0,REVERSE_FREQS},
	{"site",       1, 0, SITE},
	{"name",       1, 0, NAME},
	{"DD",         no_argument,0,DD},
	{"RR",         no_argument,0,RR},
	{"RM",         required_argument,0,RM},
	{"spc",        no_argument,0,SPC},
	{"inf",        no_argument,0,INF},
	{0, 0, 0, 0}
      };
    
      c = getopt_long(argc, argv, "hqvViM:ma:e:E:TFpIt:f:b:d:o:s:r:u:w:DSBLCx:R:",
		      long_options, &options_index);
    
      if (c == -1)
	break;

      switch (c) {
      case 'h':
	usage();
	return (0);
	break;
      case 'q':
	Pulsar::Archive::set_verbosity(0);
	break;
      case 'v':
	verbose = true;
	Pulsar::Archive::set_verbosity(2);
	break;
      case 'V':
	verbose = true;
	Pulsar::Archive::set_verbosity(3);
	break;
      case 'i':
	cout << "$Id: pam.C,v 1.60 2006/03/29 22:42:19 straten Exp $" << endl;
	return 0;
      case 'm':
	save = true;
	break;
      case 'M':
        metafile = optarg;
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
	newdm = true;
	if (sscanf(optarg, "%lf", &dm) != 1) {
	  cout << "That is not a valid dispersion measure" << endl;
	  return -1;
	}
	command += " -d ";
	command += optarg;
	break;
      case 'D':
	dedisperse = true;
	command += " -D ";
	break;
      case 'R':
	defaraday = true;
	if (sscanf(optarg, "%lf", &rm) != 1) {
	  cout << "That is not a valid rotation measure" << endl;
	  return -1;
	}
	command += " -R ";
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
	dual_parse(optarg,subint_extract_start,subint_extract_end,"x");
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

      case 207: try {
	install_receiver = Pulsar::Receiver::load (optarg);
	break;
      }
      catch (Error& error) {
	cerr << "pam: Error loading Receiver from " << optarg << endl
	     << error.get_message() << endl;
	return -1;
      }

      case 208: {
	if (sscanf(optarg, "%f", &tsub) != 1) {
	  cerr << "Invalid tsub given" << endl;
	  return -1;
	}
	tscr = true;
	break;
      }

      case TYPE:
	{
	  string s = optarg;
	  if(s=="Pulsar")     new_type = Signal::Pulsar;
	  else if(s=="PolnCal")    new_type = Signal::PolnCal;
	  else if(s=="FluxCalOn")  new_type = Signal::FluxCalOn;
	  else if(s=="FluxCalOff") new_type = Signal::FluxCalOff;
	  else if(s=="Calibrator") new_type = Signal::Calibrator;
	  else{
	    fprintf(stderr,"Unrecognised argument to --type: '%s'\n",optarg);
	    exit(-1);
	  }
	  command += " --type " + s;
	}
	break;

      case INST: instrument = optarg; break;

      case REVERSE_FREQS: reverse_freqs = true; break;

      case SITE: site = optarg; break;

      case NAME: name = optarg; break;

      case DD: dededisperse = true; break;
	  
      case RR: dedefaraday = true; break;

      case RM: rm = atof(optarg); break;

      case INF: defaraday_to_infinity = true; break;

      case SPC: scattered_power_correction = true; break;

      default:
	cout << "Unrecognised option" << endl;
      }
    }
 
   vector <string> filenames;

    if (metafile)
      stringfload (&filenames, metafile);
    else
      for (int ai=optind; ai<argc; ai++)
        dirglob (&filenames, argv[ai]);
 
    if (filenames.empty()) {
      cerr << "No filenames were specified" << endl;
      exit(-1);
    } 
  
    Reference::To<Pulsar::Archive> arch;

    if (!save) {
      cout << "Changes will not be saved. Use -m or -e to write results to disk"
	   << endl;
    }

    for (unsigned i = 0; i < filenames.size(); i++) try {
      
      if (verbose)
	cerr << "Loading " << filenames[i] << endl;
      
      arch = Pulsar::Archive::load(filenames[i]);

      if (install_receiver) {

	if (verbose)
	  cerr << "pam: Installing receiver: " << install_receiver->get_name()
	       << " in archive" << endl;

	arch->add_extension (install_receiver);

      }

      if (lin || circ) {

	Pulsar::Receiver* receiver = arch->get<Pulsar::Receiver>();

	if (!receiver)
	  cerr << "No Receiver Extension in " << filenames[i] << endl;

	else {

	  if (lin) {
	    receiver->set_basis (Signal::Linear);
	    cout << "Feed basis set to Linear" << endl;
	  }

	  if (circ) {
	    receiver->set_basis (Signal::Circular);
	    cout << "Feed basis set to Circular" << endl;
	  }

	}

      }

      if (new_cfreq) {
	float nc = arch->get_nchan();
	float bw = arch->get_bandwidth();
	float cw = bw / nc;

	float fr = new_fr - (bw / 2.0) + (cw / 2.0);
	
	for (unsigned i = 0; i < arch->get_nsubint(); i++) {
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    arch->get_Integration(i)->set_centre_frequency(j,(fr + (j*cw)));
	  }
	}
	
	arch->set_centre_frequency(new_fr);
      }

      if( new_type != Signal::Unknown )
	arch->set_type( new_type );

      if( instrument != string() ){
	Pulsar::BackendName* b = arch->get<Pulsar::BackendName>();
	if( !b )
	  fprintf(stderr,"Could not change instrument name- archive does not have BackendName extension\n");
	else
	  b->set_name(instrument);
      }

      if( site != string() ){
	if( site.size() != 1 )
	  throw Error(InvalidState,"main()",
		      "You can only use a single character for the telescope site code!");
	arch->set_telescope_code( site[0] );
      }
      if( name != string() )
	arch->set_source( name );

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
	    labels[j] = arch->get_Integration(i)->get_centre_frequency(j);
	  }
	  for (unsigned j = 0; j < arch->get_nchan(); j++) {
	    float new_frequency = labels[labels.size()-1-j];
	    arch->get_Integration(i)->set_centre_frequency(j,new_frequency);
	  }
	}
	arch->set_bandwidth(-1.0 * arch->get_bandwidth());
      }

      if( reverse_freqs ) {
	// Of course it would be nice to do this with pointers.... but oh well I guess copying will have to do HSK 27/8/04

	unsigned nchan = arch->get_nchan();

	for( unsigned isub=0; isub<arch->get_nsubint(); isub++){
	  for( unsigned ipol =0; ipol<arch->get_npol(); ipol++){
	    for( unsigned ichan=0; ichan<nchan/2; ichan++){
	      Reference::To<Pulsar::Profile> lo = arch->get_Profile(isub,ipol,ichan);	      
	      Reference::To<Pulsar::Profile> tmp = lo->clone();

	      Reference::To<Pulsar::Profile> hi = arch->get_Profile(isub,ipol,nchan-1-ichan);

	      lo->operator=(*hi);
	      hi->operator=(*tmp);
	    }
	  }
	}
	arch->set_bandwidth( -1.0 * arch->get_bandwidth() );
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

      if (scattered_power_correction) {

	Pulsar::ScatteredPowerCorrection spc;
	if (arch->get_state() == Signal::Stokes)
	  arch->convert_state(Signal::Coherence);

	for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
	  spc.transform (arch->get_Integration(isub));

      }

      if (newdm) {
	arch->set_dispersion_measure(dm);
	if (verbose)
	  cout << "Archive dispersion measure set to " << dm << endl;
      }

      if (dedisperse) {
	arch->dedisperse();
	if (verbose)
	  cout << "Archive dedipsersed" << endl;
      }

      if( dededisperse ){
	if( arch->get_dedispersed() ){
	  arch->set_dedispersed( false );
	  double dm = arch->get_dispersion_measure();
	  arch->set_dispersion_measure( -dm );
	  arch->dedisperse();
	  arch->set_dedispersed( false );
	  arch->set_dispersion_measure( dm );
	}
	else
	  fprintf(stderr,"Can't dededisperse as archive '%s' is not dedispersed\n",
		  arch->get_filename().c_str());
      }

      if (rm < 9.9e98 ) {
	arch->set_rotation_measure (rm);
	if( defaraday ){
	  Pulsar::FaradayRotation xform;
	  xform.set_rotation_measure( rm );
	  if( defaraday_to_infinity )
	    xform.set_reference_wavelength( 0.0 );
	  xform.execute( arch );
	}
	if (verbose)
	  cout << "Archive now has a RM of " << rm << endl;
      }

      if( dedefaraday ) {
	if( arch->get_faraday_corrected() ) {
	  arch->set_rotation_measure (-arch->get_rotation_measure());
	  arch->defaraday();
	  arch->set_faraday_corrected( false );
	}
	if (verbose)
	  cout << "Archive defaraday corrected for a RM of " << arch->get_rotation_measure() << endl;
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

	while ( isub<arch->get_nsubint() && isub<unsigned(subint_extract_end) ) {
	  subints.push_back( isub );
	  isub++;
	}

	Reference::To<Pulsar::Archive> extracted( arch->extract(subints) );
	extracted->set_filename( arch->get_filename() );

	arch = extracted;
      }

      if (tscr) {
	if (tsub > 0.0) {
	  unsigned factor = 
	    unsigned (tsub / arch->get_Integration(0)->get_duration());
	  if (factor == 0) {
	    cerr << "Warning: subints already too long" << endl;
	  }
	  else {
	    arch->tscrunch(factor);
	  }
	  if (verbose)
	    cout << arch->get_filename() << " tscrunched by a factor of " 
		 << factor << endl;
	}
	else if (new_nsub > 0) {
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
	    cout << "WARNING: ProcHistory command string truncated to 80 chars" 
		 << endl;
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
  
    return 0;

}
catch(Error& er) 
{
  cerr << er << endl;
  return -1;
}
catch (string& error)
{
  cerr << "exception thrown: " << error << endl;
  return -1;
}
catch (bad_alloc& ba)
{
  cerr << "Caught a bad_alloc: '" << ba.what() << "'" << endl ;
  return -1;
}
catch (exception& e)
{
  cerr << "caught an exception of type '" 
				 << typeid(e).name() << "'" << endl; 
  return -1;
}
catch(...)
{
  fprintf(stderr,"Unknown exception caught\n");
  return -1;
}




