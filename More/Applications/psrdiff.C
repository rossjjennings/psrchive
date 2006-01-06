
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
  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // name of the archive containing the standard
  char* std_filename = NULL;

  char c;
  while ((c = getopt(argc, argv, "hM:qs:vV")) != -1) 

    switch (c)  {

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
    cerr << "psrdiff: warning! standard has more than one integration" << endl;

  Pulsar::PolnProfileFit fit;
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

    for (unsigned isub=0; isub < nsub; isub++) {

      Pulsar::Integration* integration = archive->get_Integration(isub);

      for (unsigned ichan=0; ichan < nchan; ichan++) {

	Pulsar::PolnProfile* profile;
	profile = std_archive->get_Integration(isub)->new_PolnProfile(ichan);

	fit.set_standard( profile );
      
      }


    }

  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl
	 << error.get_message() << endl;
  }

  return 0;

}

