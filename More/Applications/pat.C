
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/Profile.h"
#include "Pulsar/getopt.h"

#include "Pulsar/PolnProfile.h"
#include "Pulsar/PolnProfileFit.h"
#include "Calibration/Polar.h"

#include "Phase.h"
#include "toa.h"
#include "Error.h"
#include "dirutil.h"
#include "genutil.h"

#include <cpgplot.h>

#include <string.h>

void usage ()
{
  cout << "pat - Pulsar::Archive timing \n"
    "Usage: pat [options] filenames \n"
    "  -v               Verbose mode \n" 
    "  -V               Very verbose mode \n"
    "  -i               Show revision information \n"
    "\n"
    "Preprocessing options:\n"
    "  -F               Frequency scrunch before fitting \n"
    "  -T               Time scrunch before fitting \n"
    "\n"
    "Fitting options:\n"
    "  -n harmonics     Use up to the specified number of harmonics\n"
    "  -p               Perform full polarimetric fit in Fourier domain \n"
    "  -s stdfile       Location of standard profile \n"
    "  -t               Fit in the time domain \n"
    "See http://astronomy.swin.edu.au/pulsar/software/manuals/pat.html"
       << endl;
}

int main (int argc, char *argv[])
{
  
  bool verbose = false;
  bool std_given = false;
  bool time_domain = false;
  bool full_poln = false;

  bool fscrunch = false;
  bool tscrunch = false;

  string std;

  vector<string> archives;
  vector<Tempo::toa> toas;

  int gotc = 0;

  Pulsar::PolnProfileFit fit;

  while ((gotc = getopt(argc, argv, "hiFn:ps:tTvV")) != -1) {
    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'v':
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(3);
      Calibration::Model::verbose = true;
      break;

    case 'i':
      cout << "$Id: pat.C,v 1.18 2004/04/20 13:07:36 straten Exp $" << endl;
      return 0;

    case 'F':
      fscrunch = true;
      break;

    case 'n':
      fit.set_maximum_harmonic( atoi(optarg) );
      break;

    case 'T':
      tscrunch = true;
      break;

    case 'p':
      full_poln = true;
      break;

    case 's':
      std_given = true;
      std = optarg;
      break;

    case 't':
      time_domain = true;
      break;

    default:
      cout << "Unrecognised option " << gotc << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    return -1;
  } 
  
  if (!std_given) {
    cerr << "You must specify the standard profile to use!" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> arch;
  Reference::To<Pulsar::Archive> stdarch;
  Reference::To<Pulsar::Profile> prof;

  Reference::To<const Pulsar::PolnProfile> poln_profile;

  try {
    
    stdarch = Pulsar::Archive::load(std);
    stdarch->fscrunch();
    stdarch->tscrunch();

    if (full_poln) {

      cerr << "pat: full polarization fitting with " << std << endl;
      fit.set_standard( stdarch->get_Integration(0)->new_PolnProfile(0) );
      fit.set_transformation( new Calibration::Polar );

    }
    else
      stdarch->convert_state(Signal::Intensity);

  }
  catch (Error& error) {
    cerr << "Error processing standard profile:" << endl;
    cerr << error << endl;
    return -1;
  }

  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      if (verbose)
	cerr << "Loading " << archives[i] << endl;
      
      arch = Pulsar::Archive::load(archives[i]);

      if (fscrunch)
	arch->fscrunch();
      if (tscrunch)
	arch->tscrunch();

      if (full_poln) try {

	Pulsar::Integration* integration = arch->get_Integration(0);
	poln_profile = integration->new_PolnProfile(0);

	Tempo::toa toa = fit.get_toa (poln_profile,
				      integration->get_epoch(),
				      integration->get_folding_period(),
				      arch->get_telescope_code());

        string aux = basename( arch->get_filename() );
        toa.set_auxilliary_text (aux);

	toa.unload(stdout);

      }
      catch (Error& error) {
	cerr << "pat: Error while fitting " << error << endl;
      }
      else {

	arch->convert_state (Signal::Intensity);
        arch->toas(toas, stdarch, time_domain);

      }


      for (unsigned i = 0; i < toas.size(); i++)
	toas[i].unload(stdout);

    }
    catch (Error& error) {
      fflush(stdout);
      cerr << error << endl;
    }
  }

  fflush(stdout);
  return 0;

}


