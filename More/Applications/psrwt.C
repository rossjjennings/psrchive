#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"
#include "Pulsar/FourierSNR.h"
#include "Pulsar/StandardSNR.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"

#include <cpgplot.h>

#include <iostream>
#include <unistd.h>

void usage ()
{
  cout << "weight the profiles in Pulsar::Archive(s) in various ways\n"
    "Usage: psrwt [options] file1 [file2 ...] \n"
    "Where the options are as follows \n"
    " -h        this help page \n"
    " -v        verbose output \n"
    " -V        very verbose output \n"
    " -M meta   meta names a file containing the list of files\n"
    "\n"
    "Weighting options: \n"
    " -s s/n    S/N threshold (weight zero below) default:10\n"
    " -S std    calculate S/N using a standard pulsar archive\n"
    " -m method calculate S/N using the named method: fourier or fortran\n"
    "\n"
    "Output options:\n"
    " -b scr    add scr phase bins together \n"
    " -F        add all frequency channels after weighting \n"
    " -P        add polarisations together \n"
    " -T        add all Integrations after weighting \n"
    "\n"
    "Display options:\n"
    " -D        plot each profile \n"
    " -d        display s/n but do not output weighted result\n"
    " -p phs    phase centre of off-pulse baseline (giant-pulse search)\n"
    " -w width  width of off-pulse baseline (used with -p)\n"
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

  Pulsar::FourierSNR fourier_snr;
  Pulsar::StandardSNR standard_snr;

  Reference::To<Pulsar::Archive> standard;

  int c = 0;
  const char* args = "b:DdFhm:M:Pp:Ts:S:vVw:";
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

    case 'm':

      if (strcasecmp (optarg, "fourier") == 0)
	Pulsar::Profile::snr_functor.set (&fourier_snr,
					  &Pulsar::FourierSNR::get_snr);
      
      else if (strcasecmp (optarg, "fortran") == 0)
	Pulsar::Profile::snr_functor.set (&Pulsar::snr_fortran);
      
      else {
	cerr << "psrwt: unrecognized S/N method '" << optarg << "'" << endl;
	return -1;
      }

      break;

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
      // unload_result = false;
      break;

    case 's':
      snr_threshold = atof (optarg);
      break;

    case 'S':
      Pulsar::Profile::snr_functor.set (&standard_snr,
					&Pulsar::StandardSNR::get_snr);

      cerr << "psrwt: loading standard from " << optarg << endl;
      standard = Pulsar::Archive::load (optarg);
      standard->convert_state (Signal::Intensity);

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

  Reference::To<Pulsar::Archive> archive, copy;

  Error::handle_signals ();

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    archive = Pulsar::Archive::load (filenames[ifile]);

    bool channel_standard = false;

    if ( standard )  {

      if ( standard->get_nchan() > 1 
	   && standard->get_nchan() == archive->get_nchan() ) {
	cerr << "psrwt: standard profile varies with frequency" << endl;
	channel_standard = true;
      }
      else {
	cerr << "psrwt: fscrunching standard" << endl;
	copy = standard->total();
	standard_snr.set_standard( copy->get_Profile (0,0,0) );
      }

    }

    copy = archive->clone();
    copy -> uniform_weight ();
    copy -> pscrunch();

    for (unsigned isub=0; isub < copy->get_nsubint(); isub++)
      for (unsigned ichan=0; ichan < copy->get_nchan(); ichan++) {

	if (channel_standard)
	  standard_snr.set_standard( standard->get_Profile (0,0,ichan) );

	Pulsar::Integration* subint = copy->get_Integration (isub);
	Pulsar::Profile* profile = subint->get_Profile (0,ichan);

	float snr = profile->snr ();

        if (normal)
	  cout << filenames[ifile] << "(" << isub << ", " << ichan << ")"
	       << " snr=" << snr << endl;

	else {
	    
	  double mean, variance;

	  // calculate the mean and variance at the specifed phase
	  profile->stats (snr_phase, &mean, &variance, 0, duty_cycle);
	  
	  // subtract the mean
	  *profile -= mean;
	  
	  // sum the remaining power
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
	
	if (display) {
	  cpgpage();
	  Pulsar::Plotter plotter;
	  plotter.set_subint(isub);
	  plotter.set_chan(ichan);
	  plotter.set_pol(0);
	  plotter.singleProfile(copy);
	}
	
	if (!normal)
	  continue;
	
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


