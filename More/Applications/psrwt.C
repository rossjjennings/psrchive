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

#include "dirutil.h"
#include "string_utils.h"

void usage ()
{
  cout << "weight the profiles in Pulsar::Archive(s) in various ways\n"
    "Usage: psrwt [options] file1 [file2 ...] \n"
    "Where the options are as follows \n"
    " -h        This help page \n"
    " -b scr    Bscrunch scr phase bins together \n"
    " -D        Plot each profile \n"
    " -d        Display snr but do not output weighted result\n"
    " -F        Fscrunch all frequency channels after weighting \n"
    " -M meta   meta names a file containing the list of files\n"
    " -P        add polarisations together\n"
    " -p phs    centre of the phase window used in special SNR calculation\n"
    " -s snr    SNR threshold (weight zero below) default:10\n"
    " -S std    specify the standard pulsar archive\n"
    " -T        Tscrunch all Integrations after weighting \n"
    " -w width  Width of the phase window used in conjunction with -p\n"
    " -v        Verbose output \n"
    " -V        Very verbose output \n"
       << endl;
}

int main (int argc, char** argv) 
{
  int bscrunch = -1;
  int fscrunch = -1;
  int tscrunch = -1;
  int pscrunch = -1;

  bool verbose = false;
  bool display = false;
  bool unload_result = true;
  bool normal = true;

  float snr_phase = 0.0;
  float duty_cycle = 0.15;

  char* metafile = NULL;

  double snr_threshold = 10.0;
  char* stdfile = NULL;

  int c = 0;
  const char* args = "b:DdFhM:Pp:Ts:S:vVw:";
  while ((c = getopt(argc, argv, args)) != -1)
    switch (c) {

    case 'b':
      bscrunch = atoi (optarg);
      break;

    case 'D':
      display = true;
      break;

    case 'd':
      unload_result = false;
      break;

    case 'F':
      fscrunch = 0;
      break;

    case 'h':
      usage ();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'P':
      pscrunch = 1;
      break;

    case 'p':
      snr_phase = atof (optarg);
      cerr << "psrwt: baseline phase window centre = " << snr_phase << endl;
      normal = false;
      break;

    case 's':
      snr_threshold = atof (optarg);
      break;

    case 'S':
      stdfile = optarg;
      break;

    case 'T':
      tscrunch = 0;
      break;

    case 'w':
      duty_cycle = atof(optarg);
      cerr << "psrwt: baseline phase window width = " << duty_cycle << endl;
      break;

    case 'V':
      Pulsar::Archive::verbose = true;
      Pulsar::Integration::verbose = true;
      Pulsar::Profile::verbose = true;
    case 'v':
      verbose = true;
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

  if (display) {
    cpgbeg (0, "?", 0, 0);
    cpgask(1);
    cpgsvp (0.1, 0.9, 0.05, 0.85);
    cpgsch (1.0);
  }

  // smart pointer automatically deletes instance when reference count
  Reference::To<Pulsar::Archive> archive, copy;

  Pulsar::Profile* standard = NULL;

  if (stdfile) {
    archive = Pulsar::Archive::load (stdfile);
    standard = new Pulsar::Profile (archive->get_Profile(0,0,0));
    cerr << "Standard SNR:" << standard ->snr() << endl;
  }

  Error::handle_signals ();

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

    copy = archive->clone();
    copy -> uniform_weight ();
    copy -> pscrunch();

    for (unsigned isub=0; isub < copy->get_nsubint(); isub++)
      for (unsigned ichan=0; ichan < copy->get_nchan(); ichan++) {

	Pulsar::Integration* subint = copy->get_Integration (isub);
	Pulsar::Profile* profile = subint->get_Profile (0,ichan);

	float snr = 0.0;
	
	if (standard)
	  snr = profile->snr (standard);

	else {

	  if (normal)
	    snr = profile->snr ();

	  else {

	    double mean, variance;

	    // calculate the mean and variance at the specifed phase
	    profile->stats (snr_phase, &mean, &variance, 0, duty_cycle);

	    // subtract the mean
	    *profile -= mean;

	    // sum the remaining power and divide by the rms
	    snr = profile->sum();

            // calculate the rms
            float rms = sqrt (variance);

            // find the maximum bin
            int maxbin = profile->find_max_bin();

            // get the maximum value
	    float max = profile->get_amps()[maxbin];

            double phase = double(maxbin) / double(subint->get_nbin());
            cerr << "phase=" << phase << endl;
            double seconds = phase * subint->get_folding_period();
            cerr << "seconds=" << seconds << endl;

	    // calculate the epoch of the maximum
	    MJD epoch = subint->get_epoch() + seconds;

            cerr << filenames[ifile] << " epoch=" << epoch << " max/rms="
		 << max/rms << " rms=" << rms << " sum=" << snr;

            snr /= sqrt ( profile->get_nbin() * variance );

            cerr << " snr=" << snr << endl;

	  }

	}

        if (normal)
	  cerr << filenames[ifile] << "(" << isub << ", " << ichan << ")"
	       << " snr=" << snr << endl;
	
	if (display) {
	  cpgpage();
	  Pulsar::Plotter plotter;
	  plotter.set_subint(isub);
	  plotter.set_chan(ichan);
	  plotter.set_pol(0);
	  plotter.singleProfile(copy);
	}
	
	if (snr < snr_threshold)
	  snr = 0.0;

	for (unsigned ipol=0; ipol < archive->get_npol(); ipol++)
	  archive->get_Profile (isub,ipol,ichan)->set_weight(snr*snr);
      }

    if (bscrunch > 0)
      archive -> bscrunch (bscrunch);

    if (fscrunch >= 0)
      archive -> fscrunch (fscrunch);

    if (tscrunch >= 0)
      archive -> tscrunch (tscrunch);

    if (pscrunch > 0)
      archive -> pscrunch ();

    if (unload_result)
      archive -> unload (filenames[ifile] + ".wt");

  }
  catch (Error& error) {
    cerr << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  if (display)
    cpgend();
  
  return 0;
}


