#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include "Integration.h"
#include "Error.h"
#include "BasicIntegration.h"
#include "Calibration.h"
#include "PolnCalibrator.h"
#include "FluxCalibrator.h"

// A simple command line tool for calibrating Pulsar::Archives

int main (int argc, char *argv[]) {
  
  bool verbose = false;
  bool new_database = false;
  bool display_params = false;
  bool do_fluxcal = true;
  bool do_polncal = true;
  bool write_database_file = false;

  string cals_are_here;
  vector<string> exts;

  vector<string> archives;

  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";

  while ((gotc = getopt(argc, argv, "hvVp:d:DFP")) != -1) {
    switch (gotc) {
    case 'h':
      cout << "A program for calibrating Pulsar::Archives"  << endl;
      cout << "Usage: pac [options] filenames"              << endl;
      cout << "  -v   Verbose mode"                         << endl;
      cout << "  -V   Very verbose mode"                    << endl;
      cout << "  -p   Path to CAL file directory"           << endl;
      cout << "  -e   Scan for CALs with these extensions"  << endl;
      cout << "       uses .cf and .pcal as defaults"       << endl;
      cout << "  -w   Write a new database summary file"    << endl;
      cout << "       if using -p"                          << endl;
      //cout << "  -d   Read ASCII summary (instead of -p)"   << endl;
      cout << "  -D   Display calibration model parameters" << endl;
      cout << "  -F   Calibrate fluxes only"                << endl;
      cout << "  -P   Calibrate polarisations only"         << endl;
      break;
    case 'v':
      verbose = true;
      Pulsar::Calibration::verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'p':
      cals_are_here = optarg;
      new_database = true;
      break;
    case 'd':
      cals_are_here = optarg;
      break;
    case 'D':
      display_params = true;
      break;
    case 'F':
      do_polncal = false;
      break;
    case 'P':
      do_fluxcal = false;
      break;
    case 'e':
      key = strtok (optarg, whitespace);
      while (key) {
        exts.push_back(key);
        key = strtok (NULL, whitespace);
      }
      break;
    case 'w':
      write_database_file = true;
      break;
    default:
      cout << "Unrecognised option" << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    cerr << "No archives were specified" << endl;
    exit(-1);
  } 

  // Load or generate the CAL file database
  
  Pulsar::Calibration::Database* dbase = 0;
  
  try {
    
    if (new_database) {
      if (exts.empty()) {
	exts.push_back("cf");
	exts.push_back("pcal");
      }
      cout << "Generating new database" << endl;
      
      dbase = new Pulsar::Calibration::Database (cals_are_here.c_str(), exts);
      
      if (write_database_file) {
	cout << "Writing database summary file" << endl;
	
	string temp = cals_are_here + "/database.txt";
	dbase -> unload(temp.c_str());
      }
    }
    else {
      cerr << "Reading from database summary file" << endl;
      cerr << "Not implimented yet..." << endl;
      
      //dbase = new Pulsar::Calibration::Database (cals_are_here.c_str());
    }
  }
  catch (Error& error) {
    cerr << "Error generating CAL database" << endl;
    cerr << error << endl;
    cerr << "Aborting calibration attempt" << endl;
    exit(-1);
  }
  
  // Start calibrating archives
  
  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {
      
      Pulsar::Archive* arch = Pulsar::Archive::load(archives[i]);
      cout << "Loaded archive: " << archives[i] << endl;
      
      if (do_fluxcal) {
	Pulsar::FluxCalibrator* fcal_engine = 0;
	fcal_engine = dbase->generateFluxCalibrator(arch);
	fcal_engine->calibrate(arch);
	cout << "Flux calibration complete" << endl;
      }
      
      if (do_polncal) {      
	Pulsar::PolnCalibrator* pcal_engine  = 0;
	pcal_engine = dbase->generatePolnCalibrator(arch);
	
	if (display_params)	
	  pcal_engine->store_parameters = true;
	
	pcal_engine->calibrate(arch);
	
	cout << "Polarisation calibration complete" << endl;
	
	if (display_params) {
	  //pcal_engine->model[0].display();
	}	
      }
      
      int index = archives[i].find_first_of(".", 0);
      string newname = archives[i].substr(0, index);
      newname += ".calib";
      arch->unload(newname);
      
      cout << "New file " << newname << " unloaded" << endl;
      
    }
    catch (Error& error) {
      cerr << error << endl;
    }
  }
}
