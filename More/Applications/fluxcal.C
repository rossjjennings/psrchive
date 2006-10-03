/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/psrchive.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/StandardCandles.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/OffPulseCalibrator.h"

#include "Pulsar/Database.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Config.h"

#include "string_utils.h"
#include "dirutil.h"

#include <unistd.h>
#include <iostream>
#include <algorithm>

using namespace std;

// class name of the processed calibrator archive
static string archive_class = "PSRFITS";
static string output_ext = "fluxcal";
static double interval = 5.0 * 60.0;
static Pulsar::Database* database = 0;

void usage ()
{
  cout << endl <<
    "fluxcal - produces flux calibrator solutions from sets of observations\n"
    "\n"
    "fluxcal [options] filename[s]\n"
    "options:\n"
    "  -a class     Pulsar::Archive class used to represent output\n"
    "  -c file.cfg  name of file containing standard candle information\n"
    "  -B           fix the off-pulse baseline statistics \n"
    "  -C           calibrate flux calibrator observation with itself \n"
    "  -d database  get FluxCal archives from database and file solutions\n"
    "  -e extension filename extension added to output archives\n"
    "  -i minutes   maximum number of minutes between archives in same set\n"
    "\n"
    "By default, standard candle information is read from \n" 
       << Pulsar::StandardCandles::default_filename << "\n"
    "and the maximum interval between archives in the same\n"
    "flux calibrator set is " << interval/60 << " minutes.\n"
    "\n"
    "See "PSRCHIVE_HTTP"/manuals/fluxcal for more details\n"
       << endl;
}


void unload (Pulsar::FluxCalibrator* fluxcal) try {

  Reference::To<Pulsar::Archive> archive;
  cerr << "fluxcal: creating " << archive_class << " Archive" << endl;
  archive = fluxcal->new_solution (archive_class);

  string newname = fluxcal->get_filenames ();
  char* whitespace = " ,\t\n";
  newname = replace_extension( stringtok (&newname, whitespace), output_ext );

  cerr << "fluxcal: unloading " << newname << endl;
  archive -> unload (newname);

  if (database) {
    cerr << "fluxcal: adding new entry to database" << endl;
    database->add (archive);
  }

}
catch (Error& error) {
  cerr << "fluxcal: error unloading solution\n\t"
       << error.get_message() << endl;
}


int main (int argc, char** argv) try {

  bool self_calibrate
    = Pulsar::config.get<bool> ("fluxcal::self_calibrate", false);

  bool offpulse_calibrator = false;

  Pulsar::StandardCandles* standards = 0;
  string database_filename;

  char c;
  while ((c = getopt(argc, argv, "hqvVa:BCc:d:e:i:")) != -1) 

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

    case 'B':
      offpulse_calibrator = true;
      break;

    case 'C':
      self_calibrate = true;
      break;

    case 'c':
      standards = new Pulsar::StandardCandles (optarg);
      cerr << "fluxcal: standard candles loaded from " << optarg << endl;
      break;

    case 'd':
      database = new Pulsar::Database (optarg);
      cerr << "fluxcal: database loaded from " << optarg << endl;
      database_filename = optarg;
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

  if (filenames.size() == 0 && !database) {
    cerr << "fluxcal: please specify filename[s]" << endl;
    return -1;
  }

  sort (filenames.begin(), filenames.end());

  if (database) {

    if (filenames.size() != 0) {
      cerr << "fluxcal: do not specify filename[s] with -d option" << endl;
      return -1;
    }

    // get all of the fluxcal on/off observations and sort them
    Pulsar::Database::Criterion criterion;
    vector<Pulsar::Database::Entry> entries;

    criterion = database->criterion(Pulsar::Database::any, Signal::FluxCalOn);
    database->all_matching (criterion, entries);

    criterion = database->criterion(Pulsar::Database::any, Signal::FluxCalOff);
    database->all_matching (criterion, entries);

    sort (entries.begin(), entries.end());

    if (!entries.size()) {
      cerr << "fluxcal: no FluxCalOn|Off observations in database" << endl;
      return -1;
    }

    // break them into sets

    // check for solutions (requires set identification)

    // add to filenames
    filenames.resize( entries.size() );
    for (unsigned ifile=0; ifile < filenames.size(); ifile++)
      filenames[ifile] = database->get_filename (entries[ifile]);

  }


  Reference::To<Pulsar::Archive> last;
  Reference::To<Pulsar::Archive> archive;
  Reference::To<Pulsar::FluxCalibrator> fluxcal;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    cerr << "fluxcal: loading " << filenames[ifile] << endl;
    
    archive = Pulsar::Archive::load(filenames[ifile]);

    if (self_calibrate) {

      Reference::To<Pulsar::PolnCalibrator> pcal;

      if (offpulse_calibrator)
	pcal = new Pulsar::OffPulseCalibrator (archive);
      else
	pcal = new Pulsar::SingleAxisCalibrator (archive);

      pcal->calibrate (archive);

    }

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
      if (standards)
	fluxcal->set_database (standards);

    }

    last = archive;

  }
  catch (Error& error) {
    cerr << "fluxcal: error handling " << filenames[ifile] << endl
	 << error.get_message() << endl;
  }

  if (fluxcal)
    unload (fluxcal);

  if (database) {

    string backup = database_filename + ".bkp";
    cerr << "fluxcal: backing up old database" << endl;
    rename (database_filename.c_str(), backup.c_str());
    
    cerr << "fluxcal: writing new database" << endl;
    database->unload (database_filename);

  }

  return 0;
}
catch (Error& error) {
  cerr << "fluxcal: error" << error << endl;
  return -1;
}
