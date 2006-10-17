/***************************************************************************
 *
 *   Copyright (C) 2006 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "TextInterface.h"

#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>


/////////////////////////////////////////////////////////////////////////////

namespace Pulsar {

  //! Provides a text interface for read-only access to Archive attributes
  class VapTI : public TextInterface::To<Archive> {

  public:

    //! Constructor
    VapTI ();

  };

  VapTI::VapTI ()
  {
    add( &Archive::get_filename, "file",  "Name of the file" );
    add( &Archive::get_source,   "name",  "Name of the source" );
    add( &Archive::get_nbin,     "nbin",  "Number of pulse phase bins" );
    add( &Archive::get_nchan,    "nchan", "Number of frequency channels" );
    add( &Archive::get_npol,     "npol",  "Number of polarizations" );
    add( &Archive::get_nsubint,  "nsub",  "Number of sub-integrations" );
    add( &Archive::start_time,   "stime", "Observation start time (MJD)" );
    add( &Archive::end_time,     "etime", "Observation end time (MJD)" );

    add( &Archive::integration_length,   "leng",  "Total integrated time (s)");
    add( &Archive::get_telescope_code,   "site",  "Telescope tempo code" );
    add( &Archive::get_centre_frequency, "freq",  "Centre frequency (MHz)" );
    add( &Archive::get_bandwidth,        "bw",    "Bandwidth (MHz)" );
    add( &Archive::get_coordinates,      "coord", "Sky coordinates (RA,DEC)" );

    add( &Archive::get_dispersion_measure, 
	 "dm", "Dispersion measure (pc/cm^3)" );
    
    add( &Archive::get_rotation_measure,
	 "rm", "Rotation measure (rad/m^2)" );
  
    add( &Archive::get_state,    "state", "Data state identifier" );
    add( &Archive::get_scale,    "scale", "Units of amplitude flux scale" );
    add( &Archive::get_type,     "type",  "Observation type code" );

    add( &Archive::get_dedispersed, 
	 "dmc", "Dispersion corrected (boolean)");
    
    add( &Archive::get_faraday_corrected,
	 "rmc", "Faraday Rotation corrected (boolean)" );
    
    add( &Archive::get_poln_calibrated,
	 "polc", "Polarization calibrated (boolean)" );

  };

}

/////////////////////////////////////////////////////////////////////////////


using namespace std;

void usage ()
{
  cout <<
    "A program for getting Pulsar::Archive attributes\n"
    "Usage:\n"
    "     vap -c PAR [-c PAR ...] [-E] [-p] filenames\n"
    "Where:\n"
    "\n"
    "PAR is a string containing one or more parameter names, separated by\n"
    "    commas.\n"
    "    If any whitespace is required, then the string containing it must\n"
    "    be enclosed in quotation marks. Multiple parameters may also be\n"
    "    specified by using multiple -c options.\n"
    "\n"
    "    vap -c name,freq\n"
    "\n"
    "    will print the source name and centre frequency.\n"
    "    Note that parameter names are case insensitive.\n"
    "    For a full list of parameter names, type \"vap -H\"\n"
    "\n"
    "-E  is used to print the most recent ephemeris in an archive\n"
    "\n"
    "-p  is used to print the set of polynomial coefficients\n"
       << endl;
}


using namespace Pulsar;

int main (int argc, char** argv) try {  

  // print in degrees
  Angle::default_type = Angle::Degrees;

  // suppress warnings by default
  Pulsar::Archive::set_verbosity (0);
  bool verbose = false;

  bool polycmode = false;
  bool ephemmode = false;

  vector <string> commands;

  Pulsar::VapTI tui;

  // so that a space precedes each parameter processed
  tui.set_indentation (" ");

  int gotc;
  while ((gotc = getopt (argc, argv, "c:EphHvV")) != -1)
    switch (gotc) {

    case 'E':
      ephemmode = true;
      break;

    case 'p':
      polycmode = true;
      break;

    case 'c':
      separate (optarg, commands, ",");
      break;

    case 'h':
      usage ();
      return 0;

    case 'H': {

      cout << 
	  "-----------------------------------------------------\n"
	  "Attribute Name   Description\n"
	  "-----------------------------------------------------"
	     << endl;
  
      for (unsigned i=0; i<tui.get_nattribute(); i++)
	cout << pad(16,tui.get_name(i)) 
             << " " << tui.get_description(i) << endl;
  
      return 0;

    }

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return -1;
    }
  
  vector<string> filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  string filenm;
  string cmdout;

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {

    Reference::To<Pulsar::Archive> archive;
    archive = Pulsar::Archive::load(filenames[ifile]);

    filenm = archive->get_filename();

    cout << filenm.substr(filenm.find_last_of('/')+1) << "\t";

    tui.set_instance (archive);

    for (unsigned j = 0; j < commands.size(); j++)  {
      cmdout = tui.process (commands[j]);
      cout << pad(8,cmdout.substr(cmdout.find_last_of('=')+1)) << " ";
    }

    cout << endl;

    if (ephemmode)
      cout << "\n" << archive->get_ephemeris() << endl;
    
    if (polycmode)
      cout << archive->get_model() << endl;

  } // for each archive

  catch (Error& error) {
    cout << " " << error.get_message() << endl;
  }

  return 0;

}
catch (Error& error) {
  cerr << "vap: " << error << endl;
  return -1;
}
