#include <iostream>
#include <unistd.h>

#include "Pulsar/Archive.h"
#include "tempo++.h"
#include "Error.h"

#include "genutil.h"
#include "dirutil.h"
#include "string_utils.h"

static const char* psradd_args = "b:Che:f:Fg:i:M:p:PtT:vV";

void usage () {
  cout <<
    "psradd - add archives together\n"
    "USAGE psradd [" << psradd_args << "]\n"
    " -b nbin  bin scrunch to nbin bins when each file is loaded\n"
    " -C       check that ephemerides are equal\n"
    " -h       this help page \n"
    " -f fname output filename\n"
    " -F       force append despite mismatch of header parameters\n"
    " -M meta  filename with list of files\n"
    " -p parfl load new ephemeris from 'psrfl'\n"
    " -t       make no changes to file system (testing mode)\n"
    " -T tempo system call to tempo\n"
    " -v       verbose mode \n"
    " -V       very verbose (debugging) mode \n"
    "\n"
    "AUTO ADD options\n"
    " -e ext   extension added to output filenames (default .it)\n"
    " -g sec   tscrunch+unload when time between archives > 'sec' seconds\n"
    " -i sec   tscrunch+unload when archive contains 'sec' seconds\n"
    " -P       correct for parallactic angle before tscrunch\n"
    "\n"
    "If '-i sec' is specified, '-f fname' is ignored.\n";
}



int main (int argc, char **argv)
{
  // do not make changes to file system when true
  bool testing = false;

  // verbose output
  bool verbose = false;

  // very verbose output
  bool vverbose = false;

  // if specified, bscrunch each archive to nbin
  int nbin = 0;

  // tscrunch+unload when certain limiting conditions are met
  bool auto_unload = false; 

  // auto_unload features:
  // maximum amount of data (in seconds) to integrate into one archive
  float integrate = 0.0;
  // maximum interval (in seconds) across which integration should occur
  float interval = 0.0;

  // correct for parallactic angle of receiver before tscrunch
  bool deparallactify = false;

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  string integrated_extension (".it");

  string newname;
  string parname;

  int c;

  while ((c = getopt(argc, argv, psradd_args)) != -1)  {
    switch (c)  {
    case 'h':
      usage();
      return 0;
    case 'b':
      nbin = atoi (optarg);
      break;
    case 'e':
      integrated_extension = optarg;
      break;  
    case 'f':
      newname = optarg;
      break;
    case 'g':
      if (sscanf (optarg, "%f", &interval) != 1)
	fprintf (stderr, "psradd could not parse maximum interval from '%s'\n",
		 optarg);
      auto_unload = true;
      break;
    case 'i':
      if (sscanf (optarg, "%f", &integrate) != 1)
	fprintf (stderr, "psradd could not parse integration total from '%s'\n",
		 optarg);
      auto_unload = true;
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'p':
      parname = optarg;
      break;

    case 'P':
      deparallactify = true;
      break;

    case 't':
      testing = true;
      break;

    case 'T':
      Tempo::set_system (optarg);
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      vverbose = 1;
    case 'v':
      verbose = 1;
      break;

    } 
  }

  if (!metafile && optind >= argc) {
    cerr << "psradd requires a list of archive filenames as parameters.\n";
    return -1;
  }

  if (!auto_unload && !newname.length()) {
    cerr << "psradd requires a new filename on the command line (use -f)\n";
    return -1;
  }

  psrephem neweph;
  if (!parname.empty()) {
    if (verbose)
      cerr << "psradd: loading ephemeris from '"<< parname <<"'" << endl;
    if (neweph.load (parname.c_str()) < 0) {
      cerr << "psradd could not load ephemeris from '"<< parname <<"'" << endl;
      return -1;
    }
  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  // sort the list of filenames by their MJD
  // sort_by_MJD (filenames);

  // the individual archive
  Reference::To<Pulsar::Archive> archive;

  // the accumulated total
  Reference::To<Pulsar::Archive> total;

  bool reset_total_next_load = true;
  bool correct_total = false;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) { try {

    if (verbose) cerr << "psradd: Loading [" << filenames[ifile] << "]\n";

    archive = Pulsar::Archive::load (filenames[ifile]);
    
    if (archive->integration_length() == 0) {
      cerr << "psradd: archive [" << filenames[ifile] << "] has no data\n";
      continue;
    }

    if (nbin)
      archive->bscrunch_to_nbin (nbin);

    if (reset_total_next_load) {
      if (verbose) cerr << "psradd: Setting total" << endl;
      total = archive;
      correct_total = true;
    }

    if (correct_total) {
      newname = total->get_filename() + integrated_extension;
      if (verbose)
	cerr << "psradd: New filename: '" << newname << "'" << endl;

      if (!parname.empty())
	total->set_ephemeris (neweph);

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
      // auto_unload -g: check the gap between the end of total
      // and the start of archive

      double gap = (archive->start_time() - total->end_time()).in_seconds();

      if (verbose)
	cerr << "psradd: Auto unload - gap = " << gap << " seconds" << endl;

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
	total->append (archive);
      }
      catch (Error& error) {
	cerr << "psradd: Archive::append exception:\n" << error << endl;
	if (auto_unload)
	  reset_total_current = true;
      }
    }

    if (integrate != 0.0)  {

      // ///////////////////////////////////////////////////////////////
      //
      // auto_unload -i: check that amount of data integrated in total
      // is less than the limit
      
      double integration = total->integration_length();

      if (verbose)
	cerr << "psradd: Auto unload - integration = " << integration
	     << " seconds" << endl;

      if (integration > integrate)
	reset_total_next_load = true;
    }

    if (reset_total_next_load || reset_total_current) {
      if (deparallactify)
	total->deparallactify();

      if (verbose)
	cerr << "psradd: Auto unload - tscrunch and unload " 
	     << total->integration_length() << " s archive" << endl;

      // tscrunch the archive
      total->tscrunch();

      if (!testing)
	total->unload (newname);
      
      if (reset_total_current) {
	if (verbose)
	  cerr << "psradd: Auto unload - reset total to current" << endl;
	total = archive;
	correct_total = true;
      }
    }
  }
  catch (Error& error) {
    cerr << "psradd: Error handling [" << filenames[ifile] << "]\n" 
	 << error << endl;
  }
  }

  if (!reset_total_next_load) {
    if (auto_unload)  {
      if (deparallactify)
	total->deparallactify();
      
      if (verbose) cerr << "psradd: Auto unload - tscrunching last " 
			<< total->integration_length()
			<< " seconds of data." << endl;
      total->tscrunch();
    }
    
    if (verbose)
      cerr << "psradd: Unloading archive: '" << newname << "'" << endl;
    
    if (!testing)
      total->unload (newname);
  }

  return 0;
}


