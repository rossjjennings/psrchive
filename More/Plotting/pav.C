//
// $Id: pav.C,v 1.44 2003/06/05 14:09:37 straten Exp $
//
// The Pulsar Archive Viewer
//
// A program for manipulating and visualising the data contained
// in Pulsar::Archive objects
//

#include <iostream>
#include <unistd.h>
#include <cpgplot.h>

#include "cpg_fns.h"

#include "Reference.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Passband.h"
#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"

void usage ()
{
  cout << "program to look at Pulsar::Archive(s) in various ways \n"
    "Usage: pav [options] file1 [file2 ...] \n"
    "\n"
    "Preprocessing options:\n"
    " -b scr    Bscrunch scr phase bins together\n"
    " -C        Centre the profile before plotting\n"
    " -d        Dedisperse before plotting\n"
    " -f scr    Fscrunch scr frequency channels together\n"
    " -F        Fscrunch all frequency channels\n"
    " -t src    Tscrunch scr Integrations together\n"
    " -T        Tscrunch all Integrations\n"
    " -p        Add polarisations together\n"
    " -Z        Smear profiles before plotting\n"
    "\n"
    "Selection options:\n"
    " -c map    Choose a colour map\n"
    " -M meta   meta names a file containing the list of files\n"
    " -P        Select which polarization to display\n"
    " -I        Select which subint to display\n"
    " -W        Change colour scheme to suite white background\n"
    " -z x1,x2  Zoom to this pulse phase range\n"
    "\n"
    "Plotting options:\n"
    " -A        Plot instrumental phase across the band\n"
    " -B        Display off-pulse bandpass & channel weights\n"
    " -D        Plot selected profile (chan 0, poln 0, subint 0 by default)\n"
    " -E        Display baseline spectrum\n"
    " -e        like -D with abscissa equal to time in milliseconds \n"
    " -g        Display position angle profile\n"
    " -G        Plot frequency against pulse phase\n"
    " -l        Do not display labels outside of plotting area\n"
    " -o        Plot the original passband (an Archive::Extension)\n"
    " -q        Plot a position angle frequency spectrum colour map\n"
    " -Q        Position angle frequency spectrum for on-pulse region\n"
    " -r phase  rotate the profiles by phase (in turns)\n"
    " -s        Plot S/N against frequency\n"
    " -S        Plot Stokes parameters in the Manchester style\n"
    " -X        Plot cal amplitude and phase vs frequency channel\n"
    " -Y        Display all subints (time vs pulse phase)\n"
    "\n"
    "Various options:\n"
    " -a        Print available plugin information\n"
    " -h        Display this help page \n"
    " -i        Show revision information\n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
    " -w        Use stopwatch (for benchmarking)\n"
       << endl;
}

void cpg_next ()
{
  cpgsch (1);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  cpgsvp (0.1, 0.9, 0.1, 0.9);
  cpgpage ();
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;
  
  float the_phase = 0.0;
  
  double phase = 0;
  
  bool verbose = false;
  bool zoomed = false;
  bool display = false;
  bool baseline_spectrum = false;
  bool dedisperse = false;
  bool manchester = false;
  bool greyfreq = false;
  bool stopwatch = false;
  bool hat = false;
  bool centre = false;
  bool periodplot = false;
  bool timeplot = false;
  bool calplot = false;
  bool snrplot = false;
  bool PA = false;
  bool bandpass = false;
  bool calinfo = false;
  bool pa_spectrum = false;
  bool pa_scatter = false;
  bool orig_passband = false;

  char* metafile = NULL;
  
  Pulsar::Plotter plotter;
  Pulsar::Plotter::ColourMap colour_map = Pulsar::Plotter::Heat;
  
  int c = 0;
  const char* args = "AaBb:Cc:DdEeFf:GghI:iHlm:M:Qq:opP:r:SsTt:VvwWXx:Yy:Zz:";

  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {
      
    case 'a':
      Pulsar::Archive::Agent::report ();
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

    case 'h':
      usage ();
      return 0;
      
    case 'I':
      plotter.set_subint( atoi (optarg) );
      break;
    case 'i':
      cout << "$Id: pav.C,v 1.44 2003/06/05 14:09:37 straten Exp $" << endl;
      return 0;

    case 'l':
      plotter.set_labels(false);
      break;

    case 'm':
      // macro file
      break;
    case 'M':
      metafile = optarg;
      break;

    case 'o':
      orig_passband = true;
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

    case 's':
      snrplot = true;
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

    case 'v':
      verbose = true;
      break;
    case 'V':
      Pulsar::Archive::verbose = true;
      Pulsar::Integration::verbose = true;
      Pulsar::Profile::verbose = true;
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

    case 'y':
      // y panel
      break;
    case 'Y':
      timeplot = true;
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
    case 'Z':
      hat = true;
      break;

      
    default:
      return -1; 
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

  cpgbeg (0, "?", 0, 0);
  cpgask(1);

  plotter.set_colour_map (colour_map);

  //Smart pointer
  Reference::To<Pulsar::Archive> archive;

  Error::handle_signals ();

  RealTimer clock;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

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

    if (centre)
      archive -> centre();

    if (pa_spectrum) {
      cpg_next();
      plotter.pa_vs_frequency(archive, the_phase);
    }

    if (baseline_spectrum) {
      cpg_next();
      plotter.baseline_spectrum (archive);
    }

    if (pa_scatter) {
      cpg_next();
      plotter.pa_scatter(archive);
    }

    if (bandpass) {
      cpg_next();
      plotter.bandpass(archive);
    }

    if (calinfo) {
      cpg_next();
      plotter.cal_plot(archive);
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
    
    if (timeplot) {
      cpg_next();
      plotter.phase_time (archive);
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
      plotter.set_subint(0);
      plotter.set_chan(0);
      plotter.singleProfile (archive);
    }

    if (manchester) {
      cpg_next();
      plotter.Manchester (archive);
    }

    if (periodplot) {
      cpg_next();
      cpgsvp (0.1,.95,0.1,.7);
      plotter.single_period (archive);
    }
    
    if (greyfreq) {
      cpg_next();
      plotter.phase_frequency (archive);
    }

    if (orig_passband) {

      // hacked in from bav
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
    
      unsigned nbin = passband->get_nchan ();
      vector<float> xaxis (nbin);
      for (unsigned ibin=0; ibin<nbin; ibin++)
	xaxis [ibin] = float(ibin);
    
      cpg_next();
      prettyplot (color, symbol, 1, xaxis, pband);
    
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


