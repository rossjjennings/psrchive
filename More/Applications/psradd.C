/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <math.h>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/DurationWeight.h"
#include "Pulsar/Config.h"

#include "tempo++.h"
#include "Error.h"
#include "SignalHandler.h"

#include "genutil.h"
#include "dirutil.h"
#include "string_utils.h"

bool weight_by_duration = Pulsar::config.get<bool>("weight_by_duration",false);

static const char* psradd_args = "b:c:Ce:f:FG:hiI:LM:O:p:Pqr:sS:tT:uUvVwZ:";

void usage () {
  cout <<
    "A program for adding Pulsar::Archives together\n"
    "USAGE: psradd [" << psradd_args << "] filenames\n"
    " -h          This help page\n"
    " -q          Quiet mode (suppress warnings)\n"
    " -v          Verbose mode (informational)\n"
    " -V          Very verbose mode (debugging)\n"
    " -i          Show revision information\n"
    " -L          Log results in source.log\n"
    "\n"
    " -b nbin     Scrunch to nbin bins after loading archives\n"
    " -c nchan    Scrunch to nchan frequency channels after loading archives\n"
    " -C          Check that ephemerides are equal\n"
    " -f fname    Output result to 'fname'\n"
    " -F          Force append despite mismatch of header parameters\n"
    " -M meta     Filename with list of files\n"
    " -p fname    Load new ephemeris from 'fname'\n"
    " -P          Phase align archive with total before adding\n"
    " -r freq     Add archive only if it has this centre frequency\n"
    " -s          Tscrunch result after each new file (nice on RAM)\n"
    " -t          Make no changes to file system (testing mode)\n"
    " -T tempo    System call to tempo\n"
    " -w          Weight each sub-integration by its duration \n"
    " -Z time     Only add archives that are time (+/- 0.5) seconds long\n"
    "\n"
    "AUTO ADD options:\n"
    " -e ext      Extension added to output filenames (default .it)\n"
    " -G sec      Tscrunch+unload when time between archives > 'sec' seconds\n"
    " -I sec      Tscrunch+unload when archive contains 'sec' seconds\n"
    " -O path     Path to which output files are written\n"
    " -S s/n      Tscrunch+unload when archive has this S/N\n"
    "\n"
    "Note:\n"
    " AUTO ADD options, -I, -S and -G, are incompatible with -s and -f\n"
    "\n"
    "See http://astronomy.swin.edu.au/pulsar/software/manuals/psradd.html"
       << endl;
}



int main (int argc, char **argv) try {

  // do not make changes to file system when true
  bool testing = false;

  // verbose output
  bool verbose = false;

  // very verbose output
  bool vverbose = false;

  // log the results
  bool log_results = false;

  // if specified, bscrunch each archive to nbin
  int nbin = 0;

  // if specified, fscrunch each archive to nchan
  int nchan = 0;

  // tscrunch+unload when certain limiting conditions are met
  bool auto_add = false; 

  // ensure that archive has data before adding
  bool check_has_data = true;

  // tscrunch total after each new file is appended
  bool tscrunch_total = false;

  // phase align each archive before appending to total
  bool phase_align = false;

  // auto_add features:
  // maximum amount of data (in seconds) to integrate into one archive
  float integrate = 0.0;
  // maximum signal to noise to integrate into one archive
  float max_ston = 0.0;
  // maximum interval (in seconds) across which integration should occur
  float interval = 0.0;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // extension added to auto-added output files
  string integrated_extension ("it");

  // directory to which auto-added output files are written
  string integrated_path;

  // name of the output file
  string newname;

  // name of the new ephemeris file
  string parname;

  // The centre frequency to select upon
  double centre_frequency = -1.0;

  // Only add in archives if their length matches this time
  float required_archive_length = -1.0;

  // for writing into history
  string command = "psradd";
  
  int c;  
  while ((c = getopt(argc, argv, psradd_args)) != -1)  {
    switch (c)  {

    case 'h':
      usage();
      return 0;
      
    case 'i':
      cout << "$Id: psradd.C,v 1.30 2006/03/23 21:18:33 straten Exp $" 
	   << endl;
      return 0;

    case 'b':
      nbin = atoi (optarg);
      
      command += " -b ";
      command += optarg;
      
      break;

    case 'c':
      nchan = atoi(optarg);
      
      command += " -c ";
      command += optarg;
      
      break;

    case 'e':
      integrated_extension = optarg;
      
      command += " -e ";
      command += optarg;
      
      break;  

    case 'f':
      newname = optarg;
      
      command += " -f ";
      command += optarg;
      
      break;

    case 'F':
      Pulsar::Archive::append_chronological = false;
      Pulsar::Archive::append_must_match = false;
      check_has_data = false;
      
      command += " -F";
      
      break;

    case 'G':
      if (sscanf (optarg, "%f", &interval) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as maximum interval\n";
	return -1;
      }
      auto_add = true;
      
      command += " -G ";
      command += optarg;
      
      break;

    case 'I':
      if (sscanf (optarg, "%f", &integrate) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as integration total\n";
	return -1;
      }
      auto_add = true;
      command += " -I ";
      command += optarg;
      break;

    case 'O':
      integrated_path = optarg;
      command += " -O ";
      command += optarg;
      break;

    case 'S':
      if (sscanf (optarg, "%f", &max_ston) != 1) {
	cerr << "psradd error parsing '"<< optarg <<"' as max S/N\n";
	return -1;
      }
      auto_add = true;
      command += " -S ";
      command += optarg;
      break;

    case 'L':
      log_results = true;
      break;

    case 'M':
      metafile = optarg;
      command += " -M ";
      command += optarg;
      break;

    case 'p':
      parname = optarg;
      command += " -p ";
      command += optarg;
      break;

    case 'P':
      phase_align = true;
      command += " -P";
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'r':
      centre_frequency = atof(optarg);
      command += " -r ";
      command += optarg;
      break;

    case 's':
      tscrunch_total = true;
      command += " -s";
      break;

    case 't':
      testing = true;
      break;

    case 'T':
      Tempo::set_system (optarg);
      command += " -T ";
      command += optarg;
      break;

    case 'u':
      cerr << "Will abort on Error creation" << endl;
      Error::complete_abort = true;
      SignalHandler::add_signal_to_abort_on(SIGSEGV);
      SignalHandler::add_signal_to_abort_on(SIGFPE);
      break;
    case 'U':
      cerr << "Will print message on Error creation" << endl;
      Error::verbose = true;
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      vverbose = true;
      verbose = true;
      break;

    case 'w':
      weight_by_duration = true;
      break;

    case 'Z': 
      required_archive_length = atof(optarg); 
      command += " -Z ";
      command += optarg;

      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "psradd requires a list of archive filenames as parameters.\n";
    return -1;
  }

  if (!auto_add && !newname.length()) {
    cerr << "psradd requires a new filename on the command line (use -f)\n";
    return -1;
  }

  if (auto_add && tscrunch_total) {
    cerr << "psradd cannot both AUTO ADD (-i or -g) and scrunch (-S)\n";
    return -1;
  }

  if (auto_add && newname.length())
    cerr << "psradd ignores -f when AUTO ADD features, -g or -i, are used\n";


  psrephem neweph;
  if (!parname.empty()) {

    if (verbose)
      cerr << "psradd: loading ephemeris from '"<< parname <<"'" << endl;

    if (neweph.load (parname.c_str()) < 0) {
      cerr << "psradd could not load ephemeris from '"<< parname <<"'" << endl;
      return -1;
    }

    if (vverbose)
      cerr << "psradd: ephemeris loaded=\n" << neweph << endl;

  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  // sort the list of filenames by their MJD
  // sort_by_MJD (filenames);

  // for now, sort by filename
  sort (filenames.begin(), filenames.end());

  // the individual archive
  Reference::To<Pulsar::Archive> archive;

  // the accumulated total
  Reference::To<Pulsar::Archive> total;

  bool reset_total_next_load = true;
  bool correct_total = false;

  FILE* log_file = 0;
  string log_filename;

  Reference::To<Pulsar::Weight> weight;
  if (weight_by_duration)
    weight = new Pulsar::DurationWeight;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    if (verbose) 
      cerr << "psradd: Loading [" << filenames[ifile] << "]\n";
    
    archive = Pulsar::Archive::load (filenames[ifile]);

    if (vverbose)
      for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
	cerr << isub << ": " 
	     << archive->get_model()
	  .phase( archive->get_Integration(isub)->get_epoch() ) << endl;

    if (check_has_data && archive->integration_length() == 0) {
      cerr << "psradd: archive [" << filenames[ifile] << "] has no data\n";
      continue;
    }

    if (required_archive_length > 0
	&& fabs(archive->integration_length()-required_archive_length) > 0.5)
      {
	cerr << "psradd: archive [" <<filenames[ifile] << "] not "
	     << required_archive_length << " seconds long- it was "
	     << archive->integration_length() << " seconds long" << endl;
	continue;
      }

    if( centre_frequency > 0.0 
	&& fabs(archive->get_centre_frequency()-centre_frequency) > 0.0001 )
      continue;

    if (weight)
      weight->weight (archive);

    if (nbin)
      archive->bscrunch_to_nbin (nbin);

    if( nchan )
      archive->fscrunch_to_nchan (nchan);

    if (reset_total_next_load) {
      if (verbose) cerr << "psradd: Setting total" << endl;
      total = archive;
      correct_total = true;
    }

    if (correct_total) {

      if (auto_add) {
	newname = total->get_filename() + "." + integrated_extension;
        if (!integrated_path.empty())
          newname = integrated_path + "/" + basename (newname);
      }

      if (log_results) {

	string log_name = total->get_source() + ".log";

	if (log_name != log_filename)  {
	  if (log_file) {
	    cerr << "psradd: Closing log file " << log_filename << endl;
	    fclose (log_file);
	  }
	  cerr << "psradd: Opening log file " << log_name << endl;
	  log_file = fopen (log_name.c_str(), "a");
	  if (!log_file)
	    throw Error (FailedSys, "psradd", "fopen (" + log_name + ")");
	}

	fprintf (log_file, "\npsradd %s: %s", newname.c_str(),
		 total->get_filename().c_str());
      }

      if (verbose)
	cerr << "psradd: New filename: '" << newname << "'" << endl;

      if (!parname.empty()) {

        if (verbose)
          cerr << "psradd: Installing new ephemeris" << endl;

	total->set_ephemeris (neweph);

      }

      correct_total = false;

    }

    if (reset_total_next_load)  {
      reset_total_next_load = false;
      continue;
    }

    bool reset_total_current = false;
    
    if (interval != 0.0) {
      
      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -G: check the gap between the end of total
      // and the start of archive
      
      double gap = (archive->start_time() - total->end_time()).in_seconds();
      
      if (verbose)
	cerr << "psradd: Auto add - gap = " << gap << " seconds" << endl;
      
      if (gap > interval) {
	if (verbose)
	  cerr << "psradd: gap=" << gap << " greater than interval=" 
	       << interval << endl;
	reset_total_current = true;
      }
    }

    if (!reset_total_current) {
      if (verbose)
	cerr << "psradd: appending archive to total" << endl;

      try {

	if (phase_align) {

	  Reference::To<Pulsar::Archive> standard;
	  standard = total->total();
	  Pulsar::Profile* std = standard->get_Profile(0,0,0);

	  Reference::To<Pulsar::Archive> observation;
	  observation = archive->total();
	  Pulsar::Profile* obs = observation->get_Profile(0,0,0);

	  archive->rotate_phase( obs->shift(std).get_value() );

	}

	total->append (archive);

      }
      catch (Error& error) {
	cerr << "psradd: Archive::append exception:\n" << error << endl;
	if (auto_add)
	  reset_total_current = true;
      }
      catch (...) {
        cerr << "psradd: Archive::append exception thrown" << endl;
        if (auto_add)
          reset_total_current = true;
      }

      if (log_file)
	fprintf (log_file, " %s", archive->get_filename().c_str());

    }

    if (integrate != 0.0)  {

      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -I: check that amount of data integrated in total
      // is less than the limit
      
      double integration = total->integration_length();

      if (verbose)
	cerr << "psradd: Auto add - integration = " << integration
	     << " seconds" << endl;

      if (integration > integrate)
	reset_total_next_load = true;
    }

    if (max_ston != 0.0)  {

      // ///////////////////////////////////////////////////////////////
      //
      // auto_add -S: check that S/N of the integrated data is less
      // than the limit
      
      float ston = total->total()->get_Profile(0,0,0)->snr();
      
      if (verbose)
	cerr << "psradd: Auto add - S/N = " << ston
	     << " seconds" << endl;
      
      if (ston > max_ston)
	reset_total_next_load = true;
    }
    
    if (tscrunch_total) {

      if (verbose) cerr << "psradd: tscrunch total" << endl;

      // tscrunch the archive
      total->tscrunch();
    }

    /////////////////////////////////////////////////////////////////
    // See if the archive contains a history that should be updated:

    Pulsar::ProcHistory* fitsext = total->get<Pulsar::ProcHistory>();

    if (fitsext)
      fitsext->set_command_str(command);

    if (reset_total_next_load || reset_total_current) {

      if (verbose)
	cerr << "psradd: Auto add - tscrunch and unload " 
	     << total->integration_length() << " s archive" << endl;

      total->tscrunch();

      if (!testing)
	total->unload (newname);
      
      if (reset_total_current) {
	if (verbose)
	  cerr << "psradd: Auto add - reset total to current" << endl;
	total = archive;
	correct_total = true;
      }
    }
  }
  catch (Error& error) {
    cerr << "psradd: Error handling [" << filenames[ifile] << "]\n" 
	 << error << endl;
  }

  if (!reset_total_next_load) try {

    if (auto_add)  {      
      if (verbose) cerr << "psradd: Auto add - tscrunching last " 
			<< total->integration_length()
			<< " seconds of data." << endl;
      total->tscrunch();
    }
    
    if (verbose)
      cerr << "psradd: Unloading archive: '" << newname << "'" << endl;
    
    if (!testing)
      total->unload (newname);

  }
  catch (Error& error) {
    cerr << "psradd: Error unloading total\n" << error << endl;
    return -1;
  }

  if (log_file) {
    fprintf (log_file, "\n");
    fclose (log_file);
  }

  return 0;
}
catch (Error& error) {
  cerr << "psradd: Unhandled error" << error << endl;
  return -1;
}



