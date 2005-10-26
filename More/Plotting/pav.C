//
// $Id: pav.C,v 1.101 2005/10/26 01:41:35 ateoh Exp $
//
// The Pulsar Archive Viewer
//
// A program for manipulating and visualising the data contained
// in Pulsar::Archive objects
//

 
#include <iostream>
#include <unistd.h>
#include <cpgplot.h>

#include "Reference.h"

#include "getopt.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Passband.h"
#include "Physical.h"

#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/BinLngAscOrder.h"
#include "Pulsar/FourierSNR.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"
#include "genutil.h"

using namespace std;

vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
						     bool breakup_archive);

void usage ()
{
  cout << "A program to look at Pulsar::Archive(s) in various ways\n"
    "Usage: pav [options] file1 [file2 ...]\n"
    "\n"
    "Preprocessing options:\n"
    " -b scr    Bscrunch scr phase bins together\n"
    " -C        Centre the profiles on phase zero\n"
    " -c        Don't remove baseline before plotting\n"
    " -O ang    Rotate position angle (orientation) by ang degrees\n"
    " -d        Dedisperse all channels\n"
    " -r phase  Rotate all profiles by phase (in turns)\n"
    " -f scr    Fscrunch scr frequency channels together\n"
    " -F        Fscrunch all frequency channels\n"
    " -t src    Tscrunch scr Integrations together\n"
    " -T        Tscrunch all Integrations\n"
    " -p        Add all polarisations together\n"
    " -Z        Smear profiles before plotting\n"
    " --nbin n  Bscrunch profiles to this many bins\n"
    " --spec    Replace each profile with its power spectrum\n"
    "\n"
    "Integration re-ordering (nsub = final # of subints):\n"
    " --convert_binphsperi   nsub\n"
    " --convert_binphsasc    nsub\n"
    " --convert_binlngperi   nsub\n"
    " --convert_binlngasc    nsub\n"
    "\n"
    "Functor options:\n"
    " --snr [fourier|fortran]  Calculate S/N using specified method\n"
    "\n"
    "Selection & configuration options:\n"
    " -K dev    Manually specify a plot device\n"
    " -M meta   Read a meta-file containing the files to use\n"
    " -s std    Select a standard profile (where applicable)\n"
    " -H chan   Select which frequency channel to display\n"
    " -P pol    Select which polarization to display\n"
    " -I subint Select which sub-integration to display\n"
    " -z x1,x2  Zoom to this pulse phase range\n"
    " -k f1,f2  Zoom to this frequency range\n"
    " -y s1,s2  Zoom to this subint range\n"
    " -l        Do not display labels outside of plotting area\n"
    " -N x,y    Divide the window into x by y panels\n"
    "\n"
    "Plotting options:\n"
    " -A        Plot instrumental phase across the band\n"
    " -J        Display a simple phase scrunched frequency spectrum\n"
    " -E        Display a baseline only frequency spectrum\n"
    " -B        Display off-pulse bandpass & channel weights\n"
    " -Q        Plot position angle frequency spectrum of on-pulse region\n"
    " -D        Plot a single profile (chan 0, poln 0, subint 0 by default)\n"
    " -e        Same as -D except with abscissa equal to time\n"
    " -g        Display position angle as a function of phase\n"
    " -G        Plot an image of amplitude against frequency & phase\n"
    " -m        Plot Poincare vector in spherical coordinates\n"
    " -q        Plot an image of amplitude against p.a. & frequency\n"
    " -n        Plot S/N against frequency\n"
    " -S        Plot Stokes parameters in the Manchester style\n"
    " -X        Plot cal amplitude and phase vs frequency channel\n"
    " -Y        Plot colour map of sub-integrations against pulse phase\n"
    " -R        Plot stacked sub-integration profiles\n"
    " -L        Find the width of the pulse profile\n"
    " -j        Display a simple dynamic spectrum image\n"
    " -u        Display morphological difference (requires a standard)\n"
    " -U        Plot signal-to-noise ratio as a function of pulse frequency\n"
    "\n"
    "Other plotting options: \n"
    " -W             Change colour scheme to suite white background\n"
    " --mask         Display a profile with mask\n"
    " --degree       Plot the degree of polarisation profile\n"
    " --publn        Publication quality plot\n"
    " --cmap index   Select a colour map for PGIMAG style plots\n"
    "                  The available indices are:\n"
    "                    0 -> Greyscale\n"
    "                    1 -> Inverse Greyscale\n"
    "                    2 -> Heat\n"
    "                    3 -> Cold\n"
    "                    4 -> Plasma\n"
    "                    5 -> Forest\n"
    "                    6 -> Alien Glow\n"
    " --ystretch y   Stretch y-axis limits by this factor [1.0]\n"
    " --no_prof_axes Don't show the profile axes (for -D option only)\n"
    "\n"
    "Archive::Extension options (file format specific):\n"
    " -o        Plot the original bandpass\n"
    "\n"
    "Utility options:\n"
    " -a               Print available plugin information\n"
    " -h               Display this useful help page \n"
    " -i               Show revision information\n"
    " -v               Verbose output \n"
    " -V               Very verbose output \n"
    " -w               Use stopwatch (for benchmarking)\n\n"
    " --extra fname    Treat each subint in this archive as an extra\n"
    "                  archive to process\n"
    "See http://astronomy.swin.edu.au/pulsar/software/manuals/pav.html"
       << endl;
}

void cpg_next ()
{
  cpgsch (1);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  cpgsvp (0.1, 0.9, 0.15, 0.9);
  cpgpage ();
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;

  int nbin_requested = -1;
  
  int n1 = 1;
  int n2 = 1;
  
  float    the_phase = 0.0;  
  double   phase     = 0.0;
  unsigned ronsub    = 0;
  
  bool verbose            = false;
  bool zoomed             = false;
  bool fzoomed            = false;

  bool display            = false;
  bool nesting            = false;
  bool display_axis       = false;

  bool baseline_spectrum  = false;
  bool dedisperse         = false;
  bool manchester         = false;
  bool spherical          = false;
  bool degree             = false;
  bool normalised_m       = false;
  bool phase_fourier      = false;
  bool greyfreq           = false;
  bool stopwatch          = false;
  bool hat                = false;
  bool centre             = false;
  bool periodplot         = false;
  bool subint_plot        = false;
  bool calplot            = false;
  bool snrplot            = false;
  bool PA                 = false;
  bool bandpass           = false;
  bool calinfo            = false;
  bool pa_spectrum        = false;
  bool pa_scatter         = false;
  bool orig_passband      = false;
  bool width              = false;
  bool dynam              = false;
  bool psas               = false;
  bool std_given          = false;
  bool mdiff              = false;
  bool stacked            = false;
  bool cbppo              = false;
  bool cbpao              = false;
  bool cblpo              = false;
  bool cblao              = false;
  bool mask               = false;
  bool plot_total_archive = false;
  bool profile_spectrum   = false;
	bool show_profile_axes  = true;
  float spectrum_gamma = 1.0;

  Reference::To<Pulsar::Archive> std_arch;
  Reference::To<Pulsar::Profile> std_prof;
  
  vector<string> filenames;
  
  // bools of whether archive should be broken up into its subints
  vector<int> breakup_archives; 
  
  string plot_device = "?";

  char* metafile = NULL;
  
  Pulsar::Plotter plotter;
  pgplot::ColourMap::Name colour_map = pgplot::ColourMap::Heat;
  
  Pulsar::FourierSNR fourier_snr;

  int c = 0;
  
  const char* args = 
    "AaBb:CcDdEeFf:GgH:hI:iJjK:k:LlM:mN:nO:oP:pQq:Rr:Ss:Tt:UuVvWwXx:Yy:Zz:";

  const int TOTAL = 1010;
  const int EXTRA = 1011;
  const int NBIN  = 1012;
  const int FMAX  = 1013;
  const int YSTRETCH = 1014;
  const int PROFILE_SPECTRUM = 1015;
  const int SPECTRUM_GAMMA = 1016;
	const int NO_PROFILE_AXES = 1017;

  static struct option long_options[] = {
    { "convert_binphsperi", 1, 0, 200 },
    { "convert_binphsasc",  1, 0, 201 },
    { "convert_binlngperi", 1, 0, 202 },
    { "convert_binlngasc",  1, 0, 203 },
    { "degree",             0, 0, 204 },
    { "publn",              0, 0, 205 },
    { "cmap",               1, 0, 206 },
    { "snr",                1, 0, 207 },
    { "mask",               0, 0, 208 },
    { "normal",             0, 0, 209 },
    { "total",              no_argument,       0, TOTAL},
    { "extra",              required_argument, 0, EXTRA},
    { "nbin",               required_argument, 0, NBIN},
    { "ystretch",           required_argument, 0, YSTRETCH},
    { "fmax",               required_argument, 0, FMAX},
    { "spec",               0, 0, PROFILE_SPECTRUM},
    { "specgamma",          required_argument, 0, SPECTRUM_GAMMA},
		{ "no_prof_axes",       no_argument, 0, NO_PROFILE_AXES},
    { 0, 0, 0, 0 }
  };
    
  while (1) {
    int options_index = 0;
    
    c = getopt_long(argc, argv, args,
		    long_options, &options_index);
    
    if (c == -1) 
      break;
    
    switch (c) {
    case 'a':
      Pulsar::Archive::agent_report ();
      return 0;

    case 'A':
      calplot = true;
      break;
      
    case 'b':
      bscrunch = atoi (optarg);
      break;

    case 'B':
      bandpass = true;
      break;

    case 'c':
      cerr << "pav: not removing baseline" << endl;
      plotter.set_remove_baseline (false);
      break;

    case 'C':
      centre = true;
      break;

    case 'd':
      dedisperse = true;
      break;

    case 'D':
      display = true;
      break;

    case 'e':
      periodplot = true;
      break;

    case 'E':
      baseline_spectrum = true;
      break;

    case 'f':
      fscrunch = atoi (optarg);
      break;
    case 'F':
      fscrunch = 0;
      break;

    case 'g':
      PA = true;
      break;
    case 'G':
      greyfreq = true;
      break;

    case 'H':
      plotter.set_chan( atoi (optarg) );
      break;

    case 'h':
      usage ();
      return 0;
      
    case 'I':
      plotter.set_subint( atoi (optarg) );
      break;
    case 'i':
      cout << "$Id: pav.C,v 1.101 2005/10/26 01:41:35 ateoh Exp $" << endl;
      return 0;

    case 'j':
      dynam = true;
      break;

    case 'J':
      psas = true;
      break;

    case 'K':
      plot_device = optarg;
      break;
      
    case 'l':
      plotter.set_labels(false);
      break;

    case 'L':
      width = true;
      break;

    case 'm':
      spherical = true;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'o':
      orig_passband = true;
      break;

    case 'O':
      plotter.set_orientation( atof (optarg) );
      break;

    case 'p':
      pscrunch = 1;
      break;
    case 'P':
      plotter.set_pol( atoi (optarg) );
      break;

    case 'q':
      sscanf(optarg, "%f", &the_phase);
      pa_spectrum = true;
      break;
    case 'Q':
      pa_scatter = true;
      break;

    case 'r':
      phase = atof (optarg);
      plotter.set_phase (phase);
      break;

    case 'R':
      stacked = true;
      break;

    case 'n':
      snrplot = true;
      break;

    case 's':
      try {
	std_arch = Pulsar::Archive::load(optarg);
	std_arch->tscrunch();
	std_arch->fscrunch();
	std_arch->pscrunch();
	std_prof = std_arch->get_Profile(0,0,0);
	std_given = true;
      }
      catch (Error& error) {
	cout << "Invalid standard profile:" << endl;
	cout << error << endl;
	std_given = false;
      }
      break;

    case 'S':
      manchester = true;
      break;

    case 't':
      tscrunch = atoi (optarg);
      break;
    case 'T':
      tscrunch = 0;
      break;

    case 'U':
      phase_fourier = true;
      break;

    case 'u':
      if (!std_given)
	cout << "For -u to be useful you must also specify a standard profile"
	     << endl;
      else
	mdiff = true;
      break;
     
    case 'V':
      Pulsar::Archive::set_verbosity (3);
    case 'v':
      Pulsar::Plotter::verbose = true;
      verbose = true;
      break;

    case 'w':
      stopwatch = true;
      break;
    case 'W':
      plotter.set_white_background ();
      break;

    case 'x':
      // x panel
      break;
    case 'X':
      calinfo = true;
      break;

    case 'Y':
      subint_plot = true;
      break;

    case 'z': {
      char* separator = ",";
      char* val1 = strtok (optarg, separator);
      char* val2 = strtok (NULL, separator);
      if (!val1 || !val2)  {
        cerr << "Error parsing zoom" << endl;
        return -1;
      }
      plotter.set_zoom (atof(val1), atof(val2));
      zoomed = true;
      break;
    }

    case 'k': {
      char* separator = ",";
      char* val1 = strtok (optarg, separator);
      char* val2 = strtok (NULL, separator);
      if (!val1 || !val2)  {
        cerr << "Error parsing frequencies" << endl;
        return -1;
      }
      plotter.set_freq_zoom (atof(val1), atof(val2));
      fzoomed = true;
      break;
    }

    case 'y': {
      int start_sub, finish_sub;
      dual_parse(optarg,start_sub,finish_sub,"y");
      plotter.set_sub_range (start_sub,finish_sub);
      break;
    }

    case 'N': {
      char* separator = ",";
      char* val1 = strtok (optarg, separator);
      char* val2 = strtok (NULL, separator);
      if (!val1 || !val2)  {
        cerr << "Error parsing nest parameters" << endl;
        return -1;
      }
      n1 = atoi(val1);
      n2 = atoi(val2);
      nesting = true;
      break;
    }

    case 'Z':
      hat = true;
      break;

    case 200: {
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
      
    case 201: {
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
      
    case 202: {
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
      
    case 203: {
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

    case 204: {
      degree = true;
      cout << "Plotting degree of polarisation" << endl;
      break;
    }
    case 205: {
      plotter.set_publn(true);
      cout << "Publication quality" << endl;
      break;
    }      

    case 206: {
      colour_map = (pgplot::ColourMap::Name) atoi(optarg);
      break;
    }

    case 207: {

      if (strcasecmp (optarg, "fourier") == 0)
	Pulsar::Profile::snr_strategy.set (&fourier_snr,
					  &Pulsar::FourierSNR::get_snr);
      
      else if (strcasecmp (optarg, "fortran") == 0)
	Pulsar::Profile::snr_strategy.set (&Pulsar::snr_fortran);

      else
	cerr << "pav: unrecognized snr method '" << optarg << "'" << endl;

      break;

    }
    case 208: {
      mask = true;
      break;
    }
    case 209: {
      normalised_m = true;
      cout << "Plotting normalised Stokes" << endl;	  
      break;
    }
    case TOTAL: 
      plot_total_archive = true; 
      break;
    case EXTRA: 
      filenames.push_back( optarg ); 
      breakup_archives.push_back( true); 
      break;
    case NBIN:
      nbin_requested = atoi(optarg); 
      break;
    case YSTRETCH:
      plotter.set_yaxis_stretch( atof(optarg) );
      break;
    case FMAX:
      plotter.set_y_max( atof(optarg) );
      break;
    case PROFILE_SPECTRUM:
      profile_spectrum = true;
      break;
    case SPECTRUM_GAMMA:
      spectrum_gamma = atof(optarg);
      break;
		case NO_PROFILE_AXES:
			show_profile_axes = false;
			break;
    default:
      cerr << "pav: unrecognized option" << endl;
      return -1; 
    }
  }
 
  {
    vector<string> extra_filenames;
    
    if (metafile)
      stringfload (&extra_filenames, metafile);
    else
      for (int ai=optind; ai<argc; ai++)
	dirglob (&extra_filenames, argv[ai]);

    for( unsigned i=0; i<extra_filenames.size(); i++){
      filenames.push_back( extra_filenames[i] );
      breakup_archives.push_back( false );
    }
  }

  if (filenames.size() == 0) {          
    cerr << "pav: please specify filename[s]" << endl;
    return -1;
  }

  if (cpgopen(plot_device.c_str()) < 0) {
    cout << "Error: Could not open plot device" << endl;
    return -1;
  }
  
  cpgask(1);
  
  if (nesting)
    cpgsubp(n1,n2);
  
  plotter.set_colour_map (colour_map);

  RealTimer clock;

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {
    vector<Reference::To<Pulsar::Archive> > archives = 
      get_archives(filenames[ifile],breakup_archives[ifile]);

    for( unsigned iarch = 0; iarch < archives.size(); iarch++){
      Reference::To<Pulsar::Archive> archive = archives[iarch];

      if( plot_total_archive ){
	for(unsigned ichan=0; ichan<archive->get_nchan(); ichan++)
	  fprintf(stderr,"%d\t%f\n",
		  ichan,archive->get_Integration(0)->get_weight(ichan));
	
	Reference::To<Pulsar::Archive> total(archive->total());
	total->centre();
	plotter.singleProfile(total);
	exit(0);
      }

      if (centre) {
	archive->centre();
      }
      
      if (dedisperse) {
	if (stopwatch)
	  clock.start();
	archive -> dedisperse();
	if (stopwatch) {
	  clock.stop();
	  cerr << "dedispersion toook " << clock << endl;
	}
      }
      
      if (cbppo) {
	Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
	archive->add_extension(myio); 
	myio->organise(archive, ronsub);
      }
    
      if (cbpao) {
	Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
	archive->add_extension(myio);
	myio->organise(archive, ronsub);
      }

      if (cblpo) {
	Pulsar::IntegrationOrder* myio = new Pulsar::BinLngPeriOrder();
	archive->add_extension(myio);
	myio->organise(archive, ronsub);
      }

      if (cblao) {
	Pulsar::IntegrationOrder* myio = new Pulsar::BinLngAscOrder();
	archive->add_extension(myio);
	myio->organise(archive, ronsub);
      }

      if (fscrunch >= 0) {
	if (stopwatch)
	  clock.start();
	archive -> fscrunch (fscrunch);
	if (stopwatch) {
	  clock.stop();
	  cerr << "fscrunch took " << clock << endl;
	}
      }
      
      if (tscrunch >= 0) {
	if (stopwatch)
	  clock.start();
	archive -> tscrunch (tscrunch);
	if (stopwatch) {
	  clock.stop();
	  cerr << "tscrunch took " << clock << endl;
	}
      }
      
      if (pscrunch > 0) {
	if (stopwatch)
	  clock.start();
	archive -> pscrunch ();
	if (stopwatch) {
	  clock.stop();
	  cerr << "pscrunch took " << clock.elapsedString () << endl;
	}
      }

      if( nbin_requested > 0 ) {
	bscrunch = -1;
	if (stopwatch)
	  clock.start();
	archive -> bscrunch_to_nbin (nbin_requested);
	if (stopwatch) {
	  clock.stop();
	  cerr << "bscrunch took " << clock << endl;
	}
	
      }

      if (bscrunch > 0) {
	if (stopwatch)
	  clock.start();
	archive -> bscrunch (bscrunch);
	if (stopwatch) {
	  clock.stop();
	  cerr << "bscrunch took " << clock << endl;
	}
      }

      if (profile_spectrum){
	if (stopwatch)
	  clock.start();
	archive -> get_profile_power_spectra(spectrum_gamma);
	if (stopwatch) {
	  clock.stop();
	  cerr << "--spec took " << clock << endl;
	}
      }

      if (mask) {
	Reference::To<Pulsar::Profile> prof =
	  archive->get_Profile(plotter.get_subint(),
			       plotter.get_pol(),
			       plotter.get_chan());
      
	float minphs = prof->find_min_phase();
	*prof -= (prof->mean(minphs));

	plotter.singleProfile(archive);
      
	vector<unsigned> mask = prof->get_mask();
      
	float level = (prof->max()) / 2.0;

	cpgsci(3);

	for (unsigned i = 0; i < archive->get_nbin(); i++) {
	  cpgpt1(float(i)/archive->get_nbin(), mask[i]*level, 2);
	}

	cpgsci(1);
      }
    
      if (mdiff) {
	if (std_prof) {
	  cpg_next();
	  double scale, shift;
	  Pulsar::Profile* mdp = 
	    archive->get_Profile(plotter.get_subint(),
				 plotter.get_pol(),plotter.get_chan())->
	    morphological_difference(*std_prof, scale, shift);
	  plotter.plot (mdp);
	  delete mdp;
	}
      }      
    
      if (pa_spectrum) {
	cpg_next();
	plotter.pa_vs_frequency(archive, the_phase);
      }

      if (stacked) {
	cpg_next();
	plotter.line_phase_subints(archive);
      }

      if (baseline_spectrum) {
	cpg_next();
	plotter.baseline_spectrum(archive);
      }

      if (pa_scatter) {
	cpg_next();
	plotter.pa_scatter(archive);
      }

      if (bandpass) {
	cpg_next();
	plotter.bandpass(archive);
      }
    
      if (psas) {
	cpg_next();
	plotter.total_spectrum(archive);
      }

      if (calinfo) {
	cpg_next();
	plotter.cal_plot(archive);
      }

      if (width) {
	// This routine still needs some work...

	Reference::To<Pulsar::Archive> copy = archive->total();
	copy->remove_baseline();
      
	float maxbin = float(copy->get_Profile(0,0,0)->find_max_bin());

	float error = 0.0;
	float nbin  = float(copy->get_nbin());
	float width = copy->get_Profile(0,0,0)->width(error,10.0,0.6);
	width /= 2.0;

	float period = copy->get_Integration(0)->get_folding_period();
	period *= 1000.0;

	cpg_next();
	cpgsvp(0.1, 0.9, 0.3, 0.9);
	plotter.single_period(copy);
      
	float risephs = (maxbin - nbin*width)/nbin;
	float fallphs = (maxbin + nbin*width)/nbin;
      
	if (risephs > 1.0)
	  risephs -= 1.0;
	if (risephs < 0.0)
	  risephs += 1.0;
	if (fallphs > 1.0)
	  fallphs -= 1.0;
	if (fallphs < 0.0)
	  fallphs += 1.0;
      
	float riseedge = risephs * period;
	float falledge = fallphs * period;
	cerr << falledge << ", " << riseedge << endl;
	float x1 = 0.0;
	float x2 = 0.0;
	float y1 = 0.0;
	float y2 = 0.0;
      
	cpgqwin(&x1, &x2, &y1, &y2);
	cpgsci(2);
	cpgmove(riseedge, y1);
	cpgdraw(riseedge, y2);
	cpgmove(falledge, y1);
	cpgdraw(falledge, y2);
	cpgsch(2.0);
      
	char useful[256];
	sprintf(useful, "10 percent pulse width = %.3f ms", 
		width * 2.0 * period * 1000.0);
      
	cpgmtxt("B", 4, 0.5, 0.5, useful);
	cpgsch(1.0);
	cpgsci(1);
	cpgmtxt("T", 1, 0.5, 0.5, (copy->get_source()).c_str());
      }

      if (PA) {
	cpg_next();
	plotter.pa_err(archive);
      }

      if (snrplot) {
	cpg_next();
	plotter.snrSpectrum(archive);
      }  
    
      if (calplot) {
	cpg_next();
	plotter.instrument_phase(archive, !zoomed);
      }
    
      if (subint_plot) {
	cpg_next();
	plotter.phase_subints (archive);
      }
    
      if (hat) {
	//cpg_next();
	//plotter.plot (archive -> get_Profile(0,0,0));
	//sleep(2);
	//cpgeras();

	double freq = archive->get_centre_frequency ();
	double bw = archive->get_bandwidth() / 32;
	double dm = archive->get_dispersion_measure ();
	double delay = Pulsar::dispersion_delay (dm, freq, freq+bw);
	double period = archive->get_Integration(0)->get_folding_period();
	double duty_cycle = fabs(delay/period);

	cerr << "bw=" << bw << " freq=" << freq << " dm=" << dm << endl;
	cerr << "delay=" << delay << " period=" << period << endl;
	cerr << "duty cycle=" << duty_cycle << endl;
	archive -> get_Profile(0,0,0) -> smear(duty_cycle);
	//plotter.plot (archive -> get_Profile(0,0,0));
      }
    
      if (display) {
	cpg_next();
	
	plotter.set_axes(show_profile_axes);
	plotter.singleProfile (archive);

	if (display_axis)
	  cpgbox("bcnst",0,0,"bcnst",0,0);
      }

      if (manchester) {
	cpg_next();
	plotter.Manchester (archive);
      }
      if (degree) {
	cpg_next();
	plotter.Manchester_degree (archive);
      } 
      if (normalised_m) {
	cpg_next();
	plotter.Manchester_normalised (archive);
      }
      if (spherical) {
	cpg_next();
	plotter.spherical (archive);
      }

      if (phase_fourier) {
	cpg_next();
	cpgsvp (0.2, 0.9, 0.15, 0.9);
	plotter.fourier (archive);
      }

      if (periodplot) {
	cpg_next();
	cpgsvp (0.1,.95,0.1,.7);
	plotter.single_period (archive);
      }
    
      if (dynam) {
	cpg_next();
	plotter.simple_ds(archive);
      }
    
      if (greyfreq) {
	cpg_next();
	plotter.phase_frequency (archive);
      }

      if (orig_passband) {

	const Pulsar::Passband* passband = 0;

	for (unsigned iext=0; iext < archive->get_nextension(); iext++) {

	  const Pulsar::Archive::Extension* extension;
	  extension = archive->get_extension (iext);

	  passband = dynamic_cast<const Pulsar::Passband*> (extension);

	  if (passband)
	    break;
	}

	if (!passband) {
	  cerr << "pav: Archive does not contain the Passband Extension" 
	       << endl;
	  continue;
	}

	unsigned npol = passband->get_npol ();

	vector<vector<float> > pband (npol);
	vector<int> color (npol);
	vector<int> symbol (npol, 1);
    
	for (unsigned ip=0; ip<npol; ip++) {
	  color[ip] = ip + 2;
	  pband[ip] = passband->get_passband (ip);
	}
    
	unsigned nchan = passband->get_nchan ();
	float* xaxis = new float[nchan];
	for (unsigned int i = 0; i < nchan; i++) {
	  xaxis[i] = i;
	}
      
	cpg_next();
	cpgsubp(1, npol);
      
	float min = 0;
	float max = 0;
      
	float tempmin = 0;
	float tempmax = 0;
      
	char* tempstr = new char[128];

	for (unsigned ip=0; ip<npol; ip++) {
	  min = max = tempmin = tempmax = 0;
	  minmaxval(nchan, &(pband[ip].front()), &tempmin, &tempmax);
	  if (tempmin < min)
	    min = tempmin;
	  if (tempmax > max)
	    max = tempmax;
	  for (unsigned j=0; j<nchan; j++) {
	    pband[ip][j] -= min;
	  }
	  max -= min;
	  cpgpanl(1,ip+1);
	  cpgsci(1);
	  cpgswin(0, nchan, 0, max);
	  cpgbox("ABCNT", 0.0, 0, "ABCNT", 0.0, 0);
	  sprintf(tempstr, "Original Bandpass: Polarisation %d", ip);
	  cpglab("Channel Number", "Level", tempstr);
	  cpgsci(color[ip]);
	  cpgline(nchan, xaxis, &(pband[ip].front()));
	}

	delete[] tempstr;
      }
    }

  }
  catch (Error& error) {
    cerr << "Caught Error on file '" << filenames[ifile] 
	 << "': " << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  cpgend();
  
  return 0;
}

vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
						     bool breakup_archive) {
  vector<Reference::To<Pulsar::Archive> > archives;
  Reference::To<Pulsar::Archive> arch( Pulsar::Archive::load(filename) );
  
  if( !breakup_archive ){
    archives.push_back( arch );
    return archives;
  }
  
  for( unsigned isub=0; isub<arch->get_nsubint(); isub++){
    vector<unsigned> subints(1,isub);
    archives.push_back( arch->extract(subints) );
    archives.back()->set_filename( arch->get_filename() + 
				   " subint " + make_string(isub) );
  }
  
  return archives;
}

