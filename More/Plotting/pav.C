//
// $Id: pav.C,v 1.67 2004/01/05 04:01:31 ahotan Exp $
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

#include "Pulsar/getopt.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Passband.h"

#include "Pulsar/BinaryPhaseOrder.h"
#include "Pulsar/PeriastronOrder.h"
#include "Pulsar/BinLngPeriOrder.h"
#include "Pulsar/BinLngAscOrder.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"
#include "genutil.h"

void usage ()
{
  cout << "A program to look at Pulsar::Archive(s) in various ways\n"
    "Usage: pav [options] file1 [file2 ...]\n"
    "\n"
    "Preprocessing options:\n"
    " -b scr    Bscrunch scr phase bins together\n"
    " -C        Centre the profiles on phase zero\n"
    " -d        Dedisperse all channels\n"
    " -f scr    Fscrunch scr frequency channels together\n"
    " -F        Fscrunch all frequency channels\n"
    " -t src    Tscrunch scr Integrations together\n"
    " -T        Tscrunch all Integrations\n"
    " -p        Add all polarisations together\n"
    " -Z        Smear profiles before plotting\n"
    "\n"
    "Integration re-ordering (nsub = final # of subints):\n"
    " --convert_binphsperi   nsub\n"
    " --convert_binphsasc    nsub\n"
    " --convert_binlngperi   nsub\n"
    " --convert_binlngasc    nsub\n"
    "\n"
    "Selection & configuration options:\n"
    " -K dev    Manually specify a plot device\n"
    " -c map    Select a colour map for PGIMAG style plots\n"
    " -M meta   Read a meta-file containing the files to use\n"
    " -s std    Select a standard profile (where applicable)\n"
    " -H chan   Select which frequency channel to display\n"
    " -P pol    Select which polarization to display\n"
    " -I subint Select which sub-integration to display\n"
    " -W        Change colour scheme to suite white background\n"
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
    " -O p.a.   Rotate position angle (orientation) by p.a. degrees\n"
    " -q        Plot an image of amplitude against p.a. & frequency\n"
    " -r phase  Rotate all profiles by phase (in turns)\n"
    " -n        Plot S/N against frequency\n"
    " -S        Plot Stokes parameters in the Manchester style\n"
    " -X        Plot cal amplitude and phase vs frequency channel\n"
    " -Y        Plot colour map of sub-integrations against pulse phase\n"
    " -R        Plot stacked sub-integration profiles\n"
    " -L        Find the width of the pulse profile\n"
    " -j        Display a simple dynamic spectrum image\n"
    " -u        Display morphological difference (requires a standard)\n"
    "\n"
    "Archive::Extension options (file format specific):\n"
    " -o        Plot the original bandpass\n"
    "\n"
    "Utility options:\n"
    " -a        Print available plugin information\n"
    " -h        Display this useful help page \n"
    " -i        Show revision information\n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
    " -w        Use stopwatch (for benchmarking)\n\n"
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
  
  int n1 = 1;
  int n2 = 1;
  
  float    the_phase = 0.0;  
  double   phase = 0;
  unsigned ronsub = 0;
  
  bool verbose = false;
  bool zoomed = false;
  bool fzoomed = false;
  bool szoomed = false;

  bool display = false;
  bool nesting = false;
  bool display_axis = false;

  bool baseline_spectrum = false;
  bool dedisperse = false;
  bool manchester = false;
  bool mercator = false;
  bool greyfreq = false;
  bool stopwatch = false;
  bool hat = false;
  bool centre = false;
  bool periodplot = false;
  bool subint_plot = false;
  bool calplot = false;
  bool snrplot = false;
  bool PA = false;
  bool bandpass = false;
  bool calinfo = false;
  bool pa_spectrum = false;
  bool pa_scatter = false;
  bool orig_passband = false;
  bool width = false;
  bool dynam = false;
  bool psas = false;
  bool std_given = false;
  bool mdiff = false;
  bool stacked = false;
  bool cbppo = false;
  bool cbpao = false;
  bool cblpo = false;
  bool cblao = false;

  Reference::To<Pulsar::Archive> std_arch;
  Reference::To<Pulsar::Profile> std_prof;

  string plot_device = "?";

  char* metafile = NULL;
  
  Pulsar::Plotter plotter;
  Pulsar::Plotter::ColourMap colour_map = Pulsar::Plotter::Heat;
  
  int c = 0;
  
  const char* args = 
    "AaBb:Cc:DdEeFf:GgH:hI:iJjK:k:LlM:mN:nO:oP:pQq:Rr:Ss:Tt:uVvwWXx:Yy:Zz:";
  
  while (1) {
    
    static struct option long_options[] = {
      {"convert_binphsperi", 1, 0, 200},
      {"convert_binphsasc", 1, 0, 201},
      {"convert_binlngperi", 1, 0, 202},
      {"convert_binlngasc", 1, 0, 203},
      {0, 0, 0, 0}
    };
    
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
      display_axis = true;
      display = true;
      break;
 
    case 'b':
      bscrunch = atoi (optarg);
      break;

    case 'B':
      bandpass = true;
      break;

    case 'c':
      colour_map = (Pulsar::Plotter::ColourMap) atoi(optarg);
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
      cout << "$Id: pav.C,v 1.67 2004/01/05 04:01:31 ahotan Exp $" << endl;
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
      mercator = true;
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
      
    case 'u':
      if (!std_given)
	cout << "For -u to be useful you must also specify a standard profile"
	     << endl;
      else
	mdiff = true;
      break;
      
    case 'v':
      verbose = true;
      break;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
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
      char* separator = ",";
      char* val1 = strtok (optarg, separator);
      char* val2 = strtok (NULL, separator);
      if (!val1 || !val2)  {
        cerr << "Error parsing frequencies" << endl;
        return -1;
      }
      plotter.set_sub_range (atoi(val1), atoi(val2));
      szoomed = true;
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

    default:
      return -1; 
    }
  }

  vector <string> filenames;
  
  if (metafile)
    stringfload (&filenames, metafile);
  else 
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0) {
    usage ();
    return 0;
  }
  
  if (cpgopen(plot_device.c_str()) < 0) {
    cout << "Error: Could not open plot device" << endl;
    return -1;
  }
  
  cpgask(1);
  
  if (nesting)
    cpgsubp(n1,n2);
  
  plotter.set_colour_map (colour_map);

  //Smart pointer
  Reference::To<Pulsar::Archive> archive;

  Error::handle_signals ();

  RealTimer clock;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

    if (cbppo) {
      Pulsar::IntegrationOrder* myio = new Pulsar::BinaryPhaseOrder();
      archive->add_extension(myio); 
      myio->organise(archive, ronsub);
    }
    
    if (cbpao) {
      Pulsar::IntegrationOrder* myio = new Pulsar::PeriastronOrder();
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

    if (dedisperse) {
      if (stopwatch)
	clock.start();
      archive -> dedisperse();
      if (stopwatch) {
	clock.stop();
	cerr << "dedispersion toook " << clock << endl;
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

    if (centre) {
      archive->centre();
    }

    if (stacked) {
      plotter.line_phase_subints(archive);
    }

    if (mdiff) {
      if (std_prof) {
	cpg_next();
	Pulsar::Profile* mdp = 
	  archive->get_Profile(plotter.get_subint(),
			       plotter.get_pol(),plotter.get_chan())->
	  morphological_difference(*std_prof);
	mdp->display(0,0,1,0,1,1.0,false,true);
	delete mdp;
      }
    }      
    
    if (pa_spectrum) {
      cpg_next();
      plotter.pa_vs_frequency(archive, the_phase);
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
      Reference::To<Pulsar::Archive> copy = archive->total();
      copy->remove_baseline();
      
      float max = 0.0;
      unsigned risebin = 0;
      unsigned fallbin = 0;
      max = copy->get_Profile(0,0,0)->max();
      
      max /= 2.0;
      bool transition = false;
      unsigned nbin = copy->get_nbin();
      
      for (unsigned i = 1; i < copy->get_nbin(); i++) {
	if ((copy->get_Profile(0,0,0)->mean(float(i-1)/float(nbin),0.005) <= max)
	    && (copy->get_Profile(0,0,0)->mean(float(i)/float(nbin),0.005) >= max))
	  transition = true;
	if (transition) {
	  risebin = i-1;
	  break;
	}
      }
      
      transition = false;
      
      for (unsigned i = 1; i < nbin; i++) {
	if ((copy->get_Profile(0,0,0)->mean(float(i-1)/float(nbin),0.005) >= max)
	    && (copy->get_Profile(0,0,0)->mean(float(i)/float(nbin),0.005) <= max))
	  transition = true;
	if (transition) {
	  fallbin = i-1;
	  break;
	}
      }
      
      if (risebin == 0 || fallbin == 0) {
	cout << "Could not find full width at half maximum!" << endl;
      }
      else {
	double period = copy->get_Integration(0)->get_folding_period();
	
	cpg_next();
	cpgsvp(0.1, 0.9, 0.3, 0.9);
	plotter.single_period(copy);
	
	double tbin = (period*1000.0) / double(copy->get_nbin());
	double riseedge = double(risebin) * tbin;
	double falledge = double(fallbin) * tbin;
	
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
	sprintf(useful, "Pulse FWHM = %.3f ms", 
		float(fabs(falledge-riseedge)));
	
	cpgmtxt("B", 4, 0.5, 0.5, useful);
	cpgsch(1.0);
	cpgsci(1);
	cpgmtxt("T", 1, 0.5, 0.5, (copy->get_source()).c_str());
      }
    }
    if (PA) {
      cpg_next();
      plotter.pa_profile(archive);
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
      cpg_next();
      archive -> get_Profile(0,0,0) -> display();
      sleep(2);
      cpgeras();
      archive -> get_Profile(0,0,0) -> smear(0.05);
      archive -> get_Profile(0,0,0) -> display();
    }
    
    if (display) {
      cpg_next();
      plotter.singleProfile (archive);

      if (display_axis)
	cpgbox("bcnst",0,0,"bcnst",0,0);
    }

    if (manchester) {
      cpg_next();
      plotter.Manchester (archive);
    }

    if (mercator) {
      cpg_next();
      plotter.spherical (archive);
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
	cerr << "pav: Archive does not contain the Passband Extension" << endl;
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
  catch (Error& error) {
    cerr << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  cpgend();
  
  return 0;
}


