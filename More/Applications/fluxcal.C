#include "Pulsar/FluxCalibratorDatabase.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
#include "string_utils.h"
#include "dirutil.h"

#include <unistd.h>
#include <iostream>

// class name of the processed calibrator archive
static string archive_class = "PSRFITS";
static string output_ext = "fluxcal";
static double interval = 5.0 * 60.0;

void usage ()
{
  cerr << endl <<
    "fluxcal - produces flux calibrator solutions from sets of observations\n"
    "\n"
    "fluxcal [options] filename[s]\n"
    "options:\n"
    "  -a class     Pulsar::Archive class used to represent output\n"
    "  -d database  name of file containing flux calibrator information\n"
    "  -e extension filename extension added to output archives\n"
    "  -i minutes   maximum number of minutes between archives in same set\n"
    "\n"
    "By default, flux calibrator information is read from \n" 
       << Pulsar::FluxCalibratorDatabase::default_filename << "\n"
    "and the maximum interval between archives in the same\n"
    "flux calibrator set is " << interval/60 << " minutes.\n"
       << endl;
}


void unload (Pulsar::FluxCalibrator* fluxcal)
try {

  Reference::To<Pulsar::Archive> archive;
  cerr << "fluxcal: creating " << archive_class << " Archive" << endl;
  archive = fluxcal->new_solution (archive_class);

  string newname = fluxcal->get_filenames ();
  newname = replace_extension( stringtok (&newname, ","), output_ext );

  cerr << "fluxcal: unloading " << newname << endl;
  archive -> unload (newname);
}
catch (Error& error) {
  cerr << "fluxcal: error unloading solution\n\t"
       << error.get_message() << endl;
}

int main (int argc, char** argv) try {

  Pulsar::FluxCalibratorDatabase* database = 0;

  char c;
  while ((c = getopt(argc, argv, "hqvVa:d:e:i:")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'a':
      archive_class = optarg;
      cerr << "fluxcal: will write to " << archive_class << " files" << endl;
      break;

    case 'd':
      database = new Pulsar::FluxCalibratorDatabase (optarg);
      cerr << "fluxcal: database loaded from " << optarg << endl;
      break;

    case 'e':
      output_ext = optarg;
      cerr << "fluxcal: output file extension: " << output_ext << endl;
      break;

    case 'i':
      interval = atof(optarg) * 60.0;
      cerr << "fluxcal: maximum interval between archives: " << interval/60
	   << " minutes" << endl;
      break;

    default:
      cerr << "fluxcal: invalid command line option: -" << c << endl;
      break;
    } 


  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0) {
    cerr << "fluxcal: please specify filename[s]" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> last;
  Reference::To<Pulsar::Archive> archive;
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    cerr << "fluxcal: loading " << filenames[ifile] << endl;
    
    archive = Pulsar::Archive::load(filenames[ifile]);
    
    if (fluxcal) {

      double gap = (archive->start_time() - last->end_time()).in_seconds();

      if (gap > interval) {
	cerr << "fluxcal: gap=" << time_string(gap)
	     << " > interval=" << time_string(interval) << endl;
        unload (fluxcal);
        fluxcal = 0;
      }

    }

    if (fluxcal) {

      if (Pulsar::Archive::verbose > 1)
        cerr << "fluxcal: adding observation to FluxCalibrator" << endl;

      try {
        fluxcal->add_observation (archive);
        cerr << "fluxcal: observation added to FluxCalibrator" << endl;
      }
      catch (Error& error) {
        cerr << "fluxcal: failed to add observation\n\t" 
             << error.get_message() << endl;
        unload (fluxcal);
        fluxcal = 0;
      }

    }
    
    if (!fluxcal) {

      cerr << "fluxcal: starting new FluxCalibrator" << endl;
      fluxcal = new Pulsar::FluxCalibrator (archive);
      if (database)
	fluxcal->set_database (database);

    }

    last = archive;

  }
  catch (Error& error) {
    cerr << "fluxcal: error handling " << filenames[ifile] << endl
	 << error.get_message() << endl;
  }

  if (fluxcal)
    unload (fluxcal);

  return 0;
}
catch (Error& error) {
  cerr << "fluxcal: error" << error << endl;
  return -1;
}
