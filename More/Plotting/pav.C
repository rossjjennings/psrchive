//
// $Id: pav.C,v 1.25 2003/02/10 00:32:04 pulsar Exp $
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
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Error.h"
#include "RealTimer.h"
#ifdef PSRFITS
#include "FITSError.h"
#endif

#include "dirutil.h"
#include "string_utils.h"

void usage ()
{
  cout << "program to look at Pulsar::Archive(s) in various ways \n"
    "Usage: pav [options] file1 [file2 ...] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -i        Revision information\n"
    //    " -a        Calculate TOAs of every profile \n"
    " -b scr    Bscrunch scr phase bins together \n"
    " -c map    Choose a different colour map \n"
    " -d dm     Dedisperse data to a new DM \n"
    " -D        Plot Integration 0, poln 0, chan 0 \n"
    " -G        Greyscale of profiles in frequency and phase\n"
    //    " -E f.eph  install new ephemeris given in file 'f.eph' \n"
    //    " -e xx     Output data to new file with ext xx \n"
    " -f scr    Fscrunch scr frequency channels together \n"
    " -F        Fscrunch all frequency channels \n"
    //    " -H        Print ASCII of Integration 0, poln 0, chan 0 \n"
    " -M meta   meta names a file containing the list of files\n"
    " -p        add polarisations together \n"
    " -r phase  rotate the profiles by phase (in turns)\n"
    " -S        plot Stokes parameters in the Manchester style\n"
    " -t src    Tscrunch scr Integrations together \n"
    " -T        Tscrunch all Integrations \n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
    " -w        time things \n"
    " -z x1,x2  start and end phase \n"
    " -R        Display SNR information\n"
    " -Z        Smear a profile by convolving with a hat function\n"
    " -C        Centre the profile\n"
    " -Y        Display all integrations in a time vs phase plot\n"
    " -A        Position angle spectrum plot\n"
    " -s        SNR frequency spectrum plot\n"
    " -g        Plot instrumental phase across the band\n"
       << endl;
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;

  unsigned poln = 0;

  double phase = 0;
  double new_dm = 0.0;

  bool verbose = false;
  bool zoomed = false;
  bool display = false;
  bool dedisperse = false;
  bool manchester = false;
  bool textinfo = false;
  bool greyfreq = false;
  bool stopwatch = false;
  bool hat = false;
  bool centre = false;
  bool timeplot = false;
  bool calplot = false;
  bool snrplot = false;
  bool PA = false;

  char* metafile = NULL;
  
  Pulsar::Plotter plotter;
  Pulsar::Plotter::ColourMap colour_map = Pulsar::Plotter::Heat;
  
  int c = 0;
  const char* args = "ab:c:d:DGe:E:f:FhiHm:M:pP:r:St:TvVwWx:y:RZCYz:AsgX";
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {
      
    case 'a':
      // toas
      break;
    case 'b':
      bscrunch = atoi (optarg);
      break;
    case 'c':
      colour_map = (Pulsar::Plotter::ColourMap) atoi(optarg);
      break;
    case 'd':
      new_dm = atof(optarg);
      dedisperse = true;
      break;
    case 'D':
      display = true;
      break;
    case 'G':
      greyfreq = true;
      break;
    case 'e':
      // parse ext
      break;
    case 'E':
      // parse eph file
      break;
    case 'f':
      fscrunch = atoi (optarg);
      break;
    case 'F':
      fscrunch = 0;
      break;
    case 'h':
      usage ();
      return 0;
    case 'i':
      cout << "$Id: pav.C,v 1.25 2003/02/10 00:32:04 pulsar Exp $" << endl;
      return 0;
    case 'm':
      // macro file
      break;
    case 'M':
      metafile = optarg;
      break;
    case 'p':
      pscrunch = 1;
      break;
    case 'P':
      poln = atoi (optarg);
      break;
    case 'r':
      phase = atof (optarg);
      plotter.set_phase (phase);
      break;
    case 'S':
      display = true;
      manchester = true;
      break;
    case 't':
      tscrunch = atoi (optarg);
      break;
    case 'T':
      tscrunch = 0;
      break;
    case 'V':
      Pulsar::Archive::verbose = true;
      Pulsar::Integration::verbose = true;
      Pulsar::Profile::verbose = true;
      break;
    case 'v':
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
    case 'y':
      // y panel
      break;
    case 'R':
      textinfo = true;
      break;
    case 'Z':
      hat = true;
      break;
    case 'C':
      centre = true;
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
    case 'A':
      calplot = true;
      break;
    case 's':
      snrplot = true;
      break;
    case 'g':
      PA = true;
      break;
      
    default:
      cerr << "invalid param '" << c << "'" << endl;
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

  if (display || greyfreq) {

    cpgbeg (0, "?", 0, 0);
    cpgask(1);

    if (manchester)
      cpgsvp (0.1,.95,0.1,.95);
    else if (greyfreq)
      cpgsvp (0.1,.95,0.1,.90);
    else
      cpgsvp (0.1, 0.9, 0.05, 0.85);

    cpgsch (1.0);

    plotter.set_colour_map (colour_map);
  }

  //Smart pointer
  Reference::To<Pulsar::Archive> archive;

  Error::handle_signals ();

  RealTimer clock;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

    if (dedisperse) {
      if (stopwatch)
	clock.start();
      archive -> dedisperse(new_dm, archive -> get_centre_frequency());
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
      archive -> centre();
    }
    
    if (PA) {
      cpgbeg (0, "?", 0, 0);
      cpgask(1);
      cpgsvp (0.1, 0.9, 0.1, 0.9);
      cpgeras();
      plotter.pa_profile(archive);
      cpgend();
      exit(0);
    }

    if (snrplot) {
      cpgbeg (0, "?", 0, 0);
      cpgask(1);
      cpgsvp (0.1, 0.9, 0.1, 0.9);
      cpgeras();
      plotter.snrSpectrum(archive);
      cpgend();
      exit(0);
    }  
    
    if (calplot) {
      cpgbeg (0, "?", 0, 0);
      cpgask(1);
      cpgsvp (0.1, 0.9, 0.1, 0.9);
      cpgeras();
      plotter.instrument_phase(archive, !zoomed);
      cpgend();
      exit(0);
    }
    
    if (timeplot) {
      cpgbeg (0, "?", 0, 0);
      cpgask(1);
      cpgsvp (0.1, 0.9, 0.1, 0.9);
      cpgsch (1.0);
      cpgeras();
      plotter.set_colour_map (colour_map);
      plotter.phase_time (archive);
      cpgend();
      exit(0);
    }
    
    if (hat) {
      cpgbeg (0, "?", 0, 0);
      cpgask(1);
      cpgsvp (0.1, 0.9, 0.1, 0.9);
      cpgsch (1.0);
      archive -> get_Profile(0,0,0) -> display();
      sleep(2);
      cpgeras();
      Pulsar::Profile my_profile;
      Pulsar::Profile my_hat;
      int temp1 = archive -> get_Profile(0,0,0) -> get_nbin();
      int temp2 = archive -> get_Profile(0,0,0) -> get_nbin()/32;
      cerr << "NBIN = " << temp1 << endl;
      my_hat.hat_profile(temp1, temp2);
      my_hat.display();
      sleep(2);
      cpgeras();
      my_profile.fft_convolve(archive -> get_Profile(0,0,0), &my_hat);
      my_profile.display();
      cpgend();
      exit(0);
    }
    
    if (display) {
      cpgpage();
      if (manchester)
	plotter.Manchester (archive);
      else {
	plotter.set_subint(0);
	plotter.set_pol(poln);
	plotter.set_chan(0);
	plotter.singleProfile (archive);
      }
    }
    
    if (greyfreq)
      plotter.phase_frequency (archive);

    if (textinfo) {
      archive -> pscrunch();
      archive -> tscrunch();
      archive -> fscrunch();

      double tempsnr, duration = 0.0;
      tempsnr = archive -> get_Profile(0,0,0) -> snr();
      duration = archive -> get_Integration(0) -> get_duration();

      tempsnr = tempsnr / sqrt(duration);

      cout << "SNR= " << tempsnr
	   << " CTR_FREQ= " << archive -> get_centre_frequency()
	   << " StartMJD= " << archive -> get_Integration(0) -> get_start_time()
	   << " BW= " << archive -> get_Integration(0) -> get_bandwidth()
	   << endl;
    }

  }
  catch (Error& error) {
    cerr << error << endl;
  }
#ifdef PSRFITS
  catch (FITSError& error) {
    cerr << error << endl;
  }
#endif
  catch (string& error) {
    cerr << error << endl;
  }
  
  if (display || greyfreq)
    cpgend();
  
  return 0;
}


