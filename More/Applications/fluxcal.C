#include "Pulsar/FluxCalibratorDatabase.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/Archive.h"
#include "dirutil.h"

#include <iostream>
using namespace std;

void usage ()
{
  cerr << 
    "fluxcal - produces flux calibrator solution from set of observations\n"
    "\n"
    "fluxcal [options] filename[s]\n"
    "options:\n"
    "  -a archive   Name of the archive class to which result will be output\n"
    "  -d database  Name of file containing flux calibrator information\n"
    "\n"
    "By default, flux calibrator information is read from " 
       << Pulsar::FluxCalibratorDatabase::default_filename << "\n"
       << endl;
}

int main (int argc, char** argv) try {

  char c;

  // class name of the processed calibrator archive
  string archive_class = "PSRFITS";

  Pulsar::FluxCalibratorDatabase* database = 0;

  while ((c = getopt(argc, argv, "hqvVd:")) != -1) 

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
      break;

    case 'd':
      database = new Pulsar::FluxCalibratorDatabase (optarg);
      break;

    } 


  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0) {
    cerr << "fluxcal: please specify filename[s]" << endl;
    return -1;
  }


  Reference::To<Pulsar::Archive> archive;
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  // total number of archives processed
  unsigned total = 0;
  
  for (unsigned ifile=0; ifile < filenames.size(); ifile++) {

    cerr << "fluxcal: loading " << filenames[ifile] << endl;
    
    archive = Pulsar::Archive::load(filenames[ifile]);
    
    cout << "fluxcal: loaded Archive" << endl;

    if (!fluxcal)
      fluxcal = new Pulsar::FluxCalibrator (archive);
    else
      fluxcal->add_observation (archive);

    total ++;
  }

  if (fluxcal) {

    if (database)
      fluxcal->set_database (database);

    cerr << "fluxcal: creating " << archive_class << " Archive" << endl;
    archive = fluxcal->new_solution (archive_class);

    string newname = "fluxcal.ar";

    cerr << "fluxcal: unloading " << newname << endl;
    archive -> unload (newname);

  }	

  return 0;
}
catch (Error& error) {
  cerr << "fluxcal: error" << error << endl;
  return -1;
}
