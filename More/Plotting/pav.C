/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
//
// $Id: pav.C,v 1.123 2006/10/06 21:41:57 straten Exp $
//
// The Pulsar Archive Viewer
//
// A program for manipulating and visualising the data contained
// in Pulsar::Archive objects
//

#include "Pulsar/Dispersion.h"
#include "Pulsar/psrchive.h"
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
#include "Pulsar/FaradayRotation.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "strutil.h"
#include "genutil.h"
#include "getopt.h"

#include <iostream>
#include <unistd.h>
#include <cpgplot.h>

using namespace std;

typedef struct {
  int plot_number;
  string side;
  float disp;
  float coord;
  float fjust;
  string text;
} cpgmtxt_inputs;

typedef struct {
  int plot_number;
  float x1;
  float y1;
  float x2;
  float y2;
} cpgarro_inputs;

void
open_plot_device(string& plot_device);

void
do_extras(vector<cpgmtxt_inputs> cpgmtxts,vector<cpgarro_inputs> cpgarros,
	  int plot_number);

vector<Reference::To<Pulsar::Archive> >
get_archives(string filename, bool breakup_freq);

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
    " --inf     Correct Faraday rotation to infinite frequency\n"
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
    "Configuration options:\n"
    " -K dev    Manually specify a plot device\n"
    " -M meta   Read a meta-file containing the files to use\n"
    " -s std    Select a standard profile (where applicable)\n"
    "\n"
    "Selection options:\n"
    " --all     Plot all sub-integrations; one by one\n"
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
    " --YY      Plot colour map of sub-integrations against pulse phase with yaxis as time\n"
    " -R        Plot stacked sub-integration profiles\n"
    " -L        Find the width of the pulse profile\n"
    " -j        Display a simple dynamic spectrum image\n"
    " -u        Display morphological difference (requires a standard)\n"
    " -U        Plot signal-to-noise ratio as a function of pulse frequency\n"
    "\n"
    "Other plotting options: \n"
    " -W             Change colour scheme to suite white background\n"
    " --zoom buf     Zoom onto the on-pulse with buf*width off-pulse\n"
    " --ld           Label the pulse phase axis in degrees\n"
    " --err          Include a resolution/error box in the plot\n"
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
    " --hist         Plot '-D' as histogram [false]\n"
    " --plot_qu      Plot Stokes Q and Stokes U in '-S' option instead of degree of linear\n"
    " --no_corner_labels Don't display corner labels in publication mode [do display]\n"
    " --chans arch   Treat each frequency-channel of 'arch' as a separate archive\n"
    " --cpgmtxt \"side disp, coord, fjust text\" Call cpgmtxt with these args\n"
    " --cpgmtxt \"n side disp, coord, fjust text\" Call cpgmtxt just for plot 'n' with these args\n"
    " --cpgarro \"x1 y1 x2 y2\" Call cpgarro with these args\n"
    " --cpgarro \"n x1 y1 x2 y2\" Call cpgarro just for plot 'n' with these args\n"
    " --doub         Plot two profiles\n"
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
    " --extra          Plot each subint in this archive [syn: all]\n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/pav for more details\n"
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

  bool  auto_zoom         = false;
  float auto_zoom_buffer  = 0.0;

  bool include_error      = false;
  bool longitude_degrees  = false;
  bool zoomed             = false;
  bool fzoomed            = false;

  bool display            = false;
  bool nesting            = false;

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
  bool subints_yaxis_time = false;
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
  bool all_subints        = false;
  bool show_profile_axes  = true;
  float spectrum_gamma = 1.0;
  bool do_histogram_plot  = false;
  bool plot_qu            = false;
  bool zero_wavelength    = false;
  bool double_profile     = false;

  Reference::To<Pulsar::Archive> std_arch;
  Reference::To<Pulsar::Profile> std_prof;
  
  string plot_device = "?";

  char* metafile = NULL;
  
  Pulsar::Plotter plotter;
  pgplot::ColourMap::Name colour_map = pgplot::ColourMap::Heat;
  
  Pulsar::FourierSNR fourier_snr;

  vector<string> filenames;
  vector<int> breakup_freq;

  vector<cpgmtxt_inputs> cpgmtxts;
  vector<cpgarro_inputs> cpgarros;

  int c = 0;
  
  const char* args = 
    "AaBb:CcDdEeFf:GgH:hI:iJjK:k:LlM:mN:nO:oP:pQq:Rr:Ss:Tt:UuVvWwXx:Yy:Zz:";

  const int TOTAL            = 1010;
  const int ALL_SUBINTS      = 1011;
  const int NBIN             = 1012;
  const int FMAX             = 1013;
  const int YSTRETCH         = 1014;
  const int PROFILE_SPECTRUM = 1015;
  const int SPECTRUM_GAMMA   = 1016;
  const int NO_PROFILE_AXES  = 1017;
  const int AUTO_ZOOM        = 1018;
  const int LONGITUDE_DEGREES= 1019;
  const int ERROR_BOX        = 1020;
  const int PA_AND_HALF      = 1021;
  const int HIST             = 1022;
  const int PLOT_QU          = 1023;
  const int ZERO_WAVELENGTH  = 1024;
  const int NO_CORNER_LABELS = 1025;
  const int CHANS            = 1026;
  const int CPGMTXT          = 1027;
  const int CPGARRO          = 1028;
  const int DOUB             = 1029;
  const int YY               = 1030;

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
    { "all",                no_argument,       0, ALL_SUBINTS},
    { "extra",              no_argument,       0, ALL_SUBINTS},
    { "zoom",               required_argument, 0, AUTO_ZOOM},
    { "ld",                 no_argument,       0, LONGITUDE_DEGREES},
    { "err",                no_argument,       0, ERROR_BOX},
    { "pa",                 no_argument,       0, PA_AND_HALF},
    { "nbin",               required_argument, 0, NBIN},
    { "ystretch",           required_argument, 0, YSTRETCH},
    { "fmax",               required_argument, 0, FMAX},
    { "spec",               0,                 0, PROFILE_SPECTRUM},
    { "specgamma",          required_argument, 0, SPECTRUM_GAMMA},
    { "no_prof_axes",       no_argument,       0, NO_PROFILE_AXES},
    { "hist",               no_argument,       0, HIST},
    { "plot_qu",            no_argument,       0, PLOT_QU},
    { "inf",                no_argument,       0, ZERO_WAVELENGTH},
    { "no_corner_labels",   no_argument,       0, NO_CORNER_LABELS},
    { "chans",              required_argument, 0, CHANS},
    { "cpgmtxt",            required_argument, 0, CPGMTXT},
    { "cpgarro",            required_argument, 0, CPGARRO},
    { "doub",               no_argument,       0, DOUB},
    { "double",             no_argument,       0, DOUB},
    { "YY",                 no_argument,       0, YY},
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
      cout << "$Id: pav.C,v 1.123 2006/10/06 21:41:57 straten Exp $" << endl;
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

    case YY:
      subint_plot = true;
      subints_yaxis_time = true;
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

    case ALL_SUBINTS: 
      all_subints = true;
      break;

    case AUTO_ZOOM:
      auto_zoom = true;
      auto_zoom_buffer = atof(optarg);
      break;

    case LONGITUDE_DEGREES:
      plotter.set_longitude_degrees();
      break;

    case ERROR_BOX:
      plotter.set_plot_error_box ();
      break;

    case PA_AND_HALF:
      plotter.set_pa_and_half ();
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
    case HIST:
      plotter.set_do_histogram_plot( true );
      break;
    case PLOT_QU:
      plot_qu = true;
      break;
    case ZERO_WAVELENGTH:
      zero_wavelength = true;
      break;
    case NO_CORNER_LABELS:
      plotter.set_display_corner_labels( false );
      break;
    case CHANS:
      filenames.push_back( optarg );
      breakup_freq.push_back( true );
      break;

    case CPGMTXT:
      {
	vector<string> words = stringdecimate(optarg," \t");
	cpgmtxt_inputs dummy;
	cpgmtxts.push_back( dummy );

	cpgmtxts.back().plot_number = -1;

	if( words.size()== 6 ){
	  cpgmtxts.back().plot_number = atoi(words[0].c_str());
	  words = vector<string>(words.begin()+1,words.end());
	}

	cpgmtxts.back().side = words[0];
	cpgmtxts.back().disp = atof(words[1].c_str());
	cpgmtxts.back().coord = atof(words[2].c_str());
	cpgmtxts.back().fjust = atof(words[3].c_str());
	cpgmtxts.back().text = words[4];
	break;
      }

    case CPGARRO:
      {
	vector<string> words = stringdecimate(optarg," \t");
	cpgarro_inputs dummy;
	cpgarros.push_back( dummy );
	cpgarros.back().plot_number = -1;

	if( words.size()== 5 ){
	  cpgarros.back().plot_number = atoi(words[0].c_str());
	  words = vector<string>(words.begin()+1,words.end());
	}

	cpgarros.back().x1 = atof(words[0].c_str());
	cpgarros.back().y1 = atof(words[1].c_str());
	cpgarros.back().x2 = atof(words[2].c_str());
	cpgarros.back().y2 = atof(words[3].c_str());
	break;
      }

    case DOUB: plotter.set_double_profile( true ); break;

    default:
      cerr << "pav: unrecognized option" << endl;
      return -1; 
    }
  }

  {
    vector<string> fnames;

    if (metafile)
      stringfload (&fnames, metafile);
    else
      for (int ai=optind; ai<argc; ai++)
	dirglob (&fnames, argv[ai]);

    for( unsigned i=0; i<fnames.size(); i++){
      filenames.push_back( fnames[i] );
      breakup_freq.push_back( false );
    }
  }  

  if (filenames.size() == 0) {          
    cerr << "pav: please specify filename[s]" << endl;
    return -1;
  }

  open_plot_device(plot_device);

  if( h_chomp(plot_device,"/xs") || h_chomp(plot_device,"/XSERVE") ||
      h_chomp(plot_device,"/XS") || h_chomp(plot_device,"/xserve") ||
      h_chomp(plot_device,"/png") || h_chomp(plot_device,"/PNG") )
    plotter.set_dark(true); 

  cpgask(1);
  
  if (nesting)
    cpgsubp(n1,n2);
  
  plotter.set_colour_map (colour_map);

  RealTimer clock;

  cerr << "pav: reading " << filenames.size() << " files" << endl;

  int plot_number = -1;

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {
    vector<Reference::To<Pulsar::Archive> > archives = 
      get_archives( filenames[ifile], breakup_freq[ifile] );

    for( unsigned iarch=0; iarch<archives.size(); iarch++){
      Reference::To<Pulsar::Archive> archive = archives[iarch];

      plot_number++;

      if (plotter.get_plot_error_box())
	plotter.compute_x_error (archive);
      
      unsigned nsubint = 1;
      if (all_subints)
	nsubint = archive->get_nsubint();
      
      if (plot_total_archive) {
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

	Reference::To<Pulsar::Dispersion> xform = new Pulsar::Dispersion;
	//	if( zero_wavelength )
	//xform->set_reference_wavelength( 0.0 );
	//else
	  xform->set_reference_frequency( archive->get_centre_frequency() );
	xform->set_dispersion_measure( archive->get_dispersion_measure() );
	xform->set_delta( 0.0 );
	xform->execute( archive );
	
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

      if (zero_wavelength) {
	double RM = archive->get_rotation_measure();
	if (RM == 0.0)
	  cerr << "pav: Archive RM=0; nothing to correct\n" << endl;
	else {
	  Pulsar::FaradayRotation xform;
	  xform.set_rotation_measure( RM );
	  xform.set_reference_wavelength( 0 );
	  xform.execute( archive );
	}
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

      string archive_filename = archive->get_filename();

      for (unsigned isub = 0; isub < nsubint; isub++) {
	if (all_subints) {
	  plotter.set_subint (isub);

	  archive->set_filename( archive_filename +
				 " subint " + make_string(isub) );
	}

	if (auto_zoom)
	  plotter.auto_zoom (archive, auto_zoom_buffer);

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
	  plotter.phase_subints (archive,subints_yaxis_time);
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
	  cpgsch(1.3);
	  do_extras(cpgmtxts,cpgarros,plot_number);
	}

	if (manchester) {
	  cpg_next();
	  plotter.set_svp( true );
	  plotter.set_axes( true );
	  plotter.Manchester (archive, plot_qu);
	  plotter.set_svp( false );
	  plotter.set_axes( false );
	  cpgsch(1.3);
	  do_extras(cpgmtxts,cpgarros,plot_number);
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

vector<Reference::To<Pulsar::Archive> >
get_archives(string filename, bool breakup_freq)
{
  vector<Reference::To<Pulsar::Archive> > archives;

  Reference::To<Pulsar::Archive> ar = Pulsar::Archive::load(filename);

  if( !breakup_freq ){
    archives.push_back( ar );
    return archives;
  }

  for( unsigned ichan=0; ichan<ar->get_nchan(); ichan++){
    Reference::To<Pulsar::Archive> chan = ar->clone();
    
    chan->uniform_weight( 0.0 );

    for( unsigned iint=0; iint<chan->get_nsubint(); iint++)
      chan->get_Integration(iint)->set_weight(ichan, 1.0 );

    archives.push_back( chan );
    chan->set_filename( ar->get_filename() + "_channel_" + make_string(ichan) );
  }

  return archives;
}

void
do_extras(vector<cpgmtxt_inputs> cpgmtxts,vector<cpgarro_inputs> cpgarros,
	  int plot_number)
{
  for( unsigned i=0; i<cpgmtxts.size(); i++)
    if( cpgmtxts[i].plot_number < 0 || cpgmtxts[i].plot_number == plot_number )
      cpgmtxt( cpgmtxts[i].side.c_str(),
	       cpgmtxts[i].disp,
	       cpgmtxts[i].coord,
	       cpgmtxts[i].fjust,
	       cpgmtxts[i].text.c_str());
  for( unsigned i=0; i<cpgarros.size(); i++)
    if( cpgarros[i].plot_number < 0 || cpgarros[i].plot_number == plot_number )
      cpgarro( cpgarros[i].x1, cpgarros[i].y1, cpgarros[i].x2, cpgarros[i].y2);
}

void
open_plot_device(string& plot_device)
{
  if (cpgopen(plot_device.c_str()) < 0) {
    cerr << "Error: Could not open plot device" << endl;
    exit(-1);
  }

  char value[4096];
  int length = 4095;

  cpgqinf("DEV/TYPE",value, &length);
  // fprintf(stderr,"first up got plot_device='%s'\n",plot_device.c_str());
  plot_device = string(value,value+length);
  //  fprintf(stderr,"now got plot_device='%s'\n",plot_device.c_str());
}
