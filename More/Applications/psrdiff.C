
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"
#include "MEAL/Polar.h"

#include "string_utils.h"
#include "dirutil.h"

#include <unistd.h>

void usage ()
{
  cerr << 
    "psrdiff - measure the difference between two pulse profiles \n"
    "\n"
    "psr_template [options] filename[s]\n"
    "options:\n"
    " -h               Help page \n"
    " -M metafile      Specify list of archive filenames in metafile \n"
    " -q               Quiet mode \n"
    " -v               Verbose mode \n"
    " -V               Very verbose mode \n"
    "\n"
       << endl;
}

int main (int argc, char** argv)
{
  Pulsar::Profile::default_duty_cycle = 0.10;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of the archive containing the standard
  char* std_filename = NULL;

  // write the difference to stdout
  bool plot = false;

  char c;
  while ((c = getopt(argc, argv, "dhM:qs:vV")) != -1) 

    switch (c)  {

    case 'd':
      plot = true;
      break;

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 's':
      std_filename = optarg;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;

    } 


  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty()) {
    cout << "psr_template: please specify filename[s]" << endl;
    return -1;
  } 

  if (!std_filename) {
    cout << "psr_template: please specify standard (-s std.ar)" << endl;
    return -1;
  } 

  Pulsar::Archive* std_archive = Pulsar::Archive::load( std_filename );

  unsigned std_nsub = std_archive->get_nsubint();
  unsigned std_nchan = std_archive->get_nchan();
  unsigned std_npol = std_archive->get_npol();
  unsigned std_nbin = std_archive->get_nbin();

  if (std_nsub > 1)
    cerr << "psrdiff: warning! standard has more than one subint" << endl;

  std_archive->convert_state (Signal::Stokes);
  std_archive->remove_baseline ();

  Pulsar::Integration* std_subint = std_archive->get_Integration(0);

  vector< vector< double > > std_variance;
  std_subint->baseline_stats (0, &std_variance);

  Pulsar::PolnProfileFit fit;
  fit.choose_maximum_harmonic = true;
  fit.set_transformation( new MEAL::Polar );

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load( filenames[ifile] );

    unsigned nsub = archive->get_nsubint();
    unsigned nchan = archive->get_nchan();
    unsigned npol = archive->get_npol();
    unsigned nbin = archive->get_nbin();

    if (nbin != std_nbin) {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nbin=" << nbin
	   << " != standard nbin=" << std_nbin << endl;
      continue;
    }

    if (nchan != std_nchan) {
      cerr << "psrdiff: " << archive->get_filename() << "\n    nchan=" << nchan
	   << " != standard nchan=" << std_nchan << endl;
      continue;
    }

    if (npol != std_npol) {
      cerr << "psrdiff: " << archive->get_filename() << "\n    npol=" << npol
	   << " != standard npol=" << std_npol << endl;
      continue;
    }

    archive->convert_state (Signal::Stokes);

    unsigned isub=0;

    for (isub=0; isub < nsub; isub++) {

      Pulsar::Integration* subint = archive->get_Integration(isub);

      for (unsigned ichan=0; ichan < nchan; ichan++) {

	if (std_subint->get_weight(ichan) == 0 ||
	    subint->get_weight(ichan) == 0)
	  continue;

	Reference::To<Pulsar::PolnProfile> std_profile;
	std_profile = std_subint->new_PolnProfile (ichan);

	Reference::To<Pulsar::PolnProfile> profile;
	profile = subint->new_PolnProfile (ichan);

	fit.set_standard (std_profile);
        fit.fit (profile);
  
	Estimate<double> pulse_phase = fit.get_phase ();
	Jones<double> transformation = fit.get_transformation()->evaluate();

	cerr << "ichan=" << ichan << " shift=" << pulse_phase 
	     << " xform=" << transformation << endl;

	profile->rotate_phase (pulse_phase.get_value());
	profile->transform (inv(transformation));

      }

    }

    archive->remove_baseline ();

    double total_chisq = 0.0;
    unsigned count = 0;

    for (isub=0; isub < nsub; isub++) {

      Pulsar::Integration* subint = archive->get_Integration(isub);

      vector< vector< double > > variance;
      subint->baseline_stats (0, &variance);

      for (unsigned ichan=0; ichan < nchan; ichan++) {

	if (std_subint->get_weight(ichan) == 0 ||
	    subint->get_weight(ichan) == 0)
	  continue;

	for (unsigned ipol=0; ipol < npol; ipol++) {

	  double var = variance[ipol][ichan] + std_variance[ipol][ichan];

	  float* std_amps = std_subint->get_Profile(ipol,ichan)->get_amps();
	  float* amps = subint->get_Profile (ipol,ichan)->get_amps();

	  double diff = 0;
	  for (unsigned ibin = 0; ibin < nbin; ibin++) {
	    double val = std_amps[ibin] - amps[ibin];
	    diff += val * val;
	  }

	  double reduced_chisq = diff / (nbin * var);
	  
	  cout << "isub=" << isub << " ichan=" << ichan << " ipol=" << ipol
	       << " var=" << var << " chisq=" << reduced_chisq << endl;

	  total_chisq += reduced_chisq;
	  count ++;

	}


	if (plot) {

	  double rms = sqrt(variance[0][ichan]);

	  for (unsigned ibin = 0; ibin < nbin; ibin++) {
	    
	    cout << ibin;
	    
	    for (unsigned ipol=0; ipol < npol; ipol++) {
	      
	      float* stds = std_subint->get_Profile(ipol,ichan)->get_amps();
	      float* amps = subint->get_Profile (ipol,ichan) -> get_amps();
	      
	      cout << " " << amps[ibin] - stds[ibin];
	      
	    }
	    
	    cout << endl;
	    
	  }

	}

      }

    }

    cout << "avg. reduced chisq=" << total_chisq/count << endl;
    
  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl
	 << error.get_message() << endl;
  }

  return 0;

}

