#include <iostream>
#include <unistd.h>
#include <cpgplot.h>

#include "Archive.h"
#include "Integration.h"
#include "Profile.h"
#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"

void usage ()
{
  cout << "program to look at Pulsar::Archive(s) in various ways \n"
    "Usage: treduce [options] file1 [file2 ...] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -a        Calculate TOAs of every profile \n"
    " -b scr    Bscrunch scr phase bins together \n"
    " -c        Correct data for bad ephemeris \n"
    " -d dm     Dedisperse data at a new dm \n"
    " -D        Plot Integration 0, poln 0, chan 0 \n"
    " -G        Greyscale of profiles in frequency and phase\n"
    " -E f.eph  install new ephemeris given in file 'f.eph' \n"
    " -e xx     Output data to new file with ext xx \n"
    " -f scr    Fscrunch scr frequency channels together \n"
    " -F        Fscrunch all frequency channels \n"
    " -H        Print ASCII of Integration 0, poln 0, chan 0 \n"
    " -m macro  Process data using steps in macro \n"
    " -M meta   meta names a file containing the list of files\n"
    " -p        add polarisations together \n"
    " -r phase  rotate the profiles by phase (in turns)\n"
    " -S        plot pulsar dynamic spectra:  frequency vs time. \n"
    " -t src    Tscrunch scr Integrations together \n"
    " -T        Tscrunch all Integrations \n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
    " -w        time things \n"
    " -x nx     plot nx profiles across screen \n"
    " -y ny     plot ny profiles down screen\n"
    " -R        Display SNR information\n"
       << endl;
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;

  double phase = 0;

  bool verbose = false;
  bool display = false;
  bool textinfo = false;
  bool greyfreq = false;
  bool stopwatch = false;

  char* metafile = NULL;

  int c = 0;
  const char* args = "ab:cd:DGe:E:f:FHm:M:pr:St:TvVwx:y:R";
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'a':
      // toas
      break;
    case 'b':
      bscrunch = atoi (optarg);
      break;
    case 'c':
      // correct
      break;
    case 'd':
      // parse dm
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
    case 'H':
      // ascii
      break;
    case 'm':
      // macro file
      break;
    case 'M':
      metafile = optarg;
      break;
    case 'p':
      pscrunch = 1;
      break;
    case 'r':
      phase = atof (optarg);
      break;
    case 'S':
      // dynamic spectra
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
    case 'v':
      verbose = true;
      break;
    case 'w':
      stopwatch = true;
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
  }

  if (display) {
    cpgsvp (0.05, 0.95, 0.0, 0.8);
    cpgsch (2.0);
  }

  //#ifdef HEAT

  //float heat_l[] = {0.0, 0.2, 0.4, 0.6, 1.0};
  //float heat_r[] = {0.0, 0.5, 1.0, 1.0, 1.0};
  //float heat_g[] = {0.0, 0.0, 0.5, 1.0, 1.0};
  //float heat_b[] = {0.0, 0.0, 0.0, 0.3, 1.0};
  
  //cpgctab(heat_l, heat_r, heat_g, heat_b, 4, 1.0, 0.5);

  //#endif

  Pulsar::Archive* archive = 0;

  Pulsar::Error::handle_signals ();

  RealTimer clock;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

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

    if (display) {
      cpgpage();
      archive -> display(0,0,0,phase);
    }
    
    if (greyfreq) {
      string tempstr = archive -> get_source();
      Pulsar::plot_color (archive -> get_Integration (0), tempstr);
    }
    
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

    delete archive; archive = 0;
  }
  catch (Pulsar::Error& error) {
    cerr << error << endl;
    if (archive)
      delete archive; archive = 0;
  }
  catch (string& error) {
    cerr << error << endl;
    if (archive)
      delete archive; archive = 0;
  }
  
  if (display || greyfreq)
    cpgend();
  
  return 0;
}


