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

// A command line tool for calibrating Pulsar::Archives

void usage ()
{
  cout << "A program for calibrating Pulsar::Archives\n"
    "Usage: pac [options] filenames\n"
    "  -v   Verbose mode\n"
    "  -V   Very verbose mode\n"
    "\n"
    "  -p   Path to CAL file directory\n"
    "  -e   Scan for files with these extensions\n"
    "       uses .cf and .pcal as defaults\n"
    "  -u   Use this extension when unloading results\n"
    "  -w   Write a new database summary file if using -p\n"
    "  -W   Same as -w but exit after writing summary\n"
    "  -d   Read ASCII summary (instead of -p)\n"   
    "  -c   Do not try to match sky coordinates\n"
    "  -i   Do not try to match instruments\n"
    "  -t   Do not try to match times\n"
    "  -f   Do not try to match frequencies\n"
    "  -b   Do not try to match bandwidths\n"
    "  -o   Do not try to match obs types\n"
    "  -D   Display calibration model parameters\n"
    "  -F   Calibrate fluxes only\n"
    "  -P   Calibrate polarisations only\n"
    "\n"
    "  -S   Use the SingleAxis(t)Polar (SAtP selfcal) model\n"
    "  -s   Use the Single Axis Model (default)\n"
    "  -q   Use the Polar Model\n"
       << endl;
}

int main (int argc, char *argv[]) {
    
  bool verbose = false;
  bool new_database = true;
  bool display_params = false;
  bool do_fluxcal = true;
  bool do_polncal = true;
  bool do_selfcal = false;
  bool write_database_file = false;
  bool summary_only = false;

  bool test_instr = true;
  bool test_coords = true;
  bool test_times = true;
  bool test_frequency = true;
  bool test_bandwidth = true;
  bool test_obstype = true;
  
  Pulsar::Calibration::Database::ModelTypes m;
  m = Pulsar::Calibration::Database::SingleAxis;
  
  Error::verbose = true;

  string cals_are_here = "./";
  string unload_ext = "calib";
  vector<string> exts;

  vector<string> archives;

  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";

  while ((gotc = getopt(argc, argv, "hvVp:e:u:d:wWcitfboDFPsSq")) != -1) {
    switch (gotc) {
    case 'h':
      usage ();
      return 0;
    case 'v':
      verbose = true;
      Pulsar::Calibration::verbose = true;
      Pulsar::Calibrator::verbose = true;
      break;
    case 'V':
      verbose = true;
      Pulsar::Calibration::verbose = true;
      Pulsar::Calibrator::verbose = true;
      Pulsar::Archive::set_verbosity(1);
      break;
    case 'p':
      cals_are_here = optarg;
      break;
    case 'd':
      cals_are_here = optarg;
      new_database = false;
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
    case 'u':
      unload_ext = optarg;
      break;
    case 'w':
      if (new_database)
	write_database_file = true;
      break;
    case 'W':
      write_database_file = true;
      summary_only = true;
      break;
    case 'c':
      test_coords = false;
      break;
    case 'i':
      test_instr = false;
      break;
    case 't':
      test_times = false;
      break;
    case 'f':
      test_frequency = false;
      break;
    case 'b':
      test_bandwidth = false;
      break;
    case 'o':
      test_obstype = false;
      break;
    case 'S':
      do_selfcal = true;
      break;
    case 's':
      m = Pulsar::Calibration::Database::SingleAxis;
      break;
    case 'q':
      m = Pulsar::Calibration::Database::Polar;
      break;

    default:
      cout << "Unrecognised option!" << endl;
    }
  }
  
  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    if (!summary_only) {
      cerr << "No archives were specified!" << endl;
      exit(-1);
    }
  } 

  // Load or generate the CAL file database
  
  Pulsar::Calibration::Database* dbase = 0;
  
  try {
    
    if (new_database) {
      
      exts.push_back("cf");
      exts.push_back("pcal");
      
      cout << "Generating new database" << endl;
      
      dbase = new Pulsar::Calibration::Database (cals_are_here.c_str(), exts);
      
      dbase -> test_inst(test_instr);
      dbase -> test_posn(test_coords);
      dbase -> test_time(test_times);
      dbase -> test_freq(test_frequency);
      dbase -> test_bw(test_bandwidth);
      dbase -> test_obst(test_obstype);

      if (dbase->size() <= 0) {
	cerr << "No CAL files found!" << endl;
	return -1;
      }

      if (verbose)
	cerr << dbase->size() << " Calibrator Archives found" << endl;

      if (write_database_file) {
	cout << "Writing database summary file" << endl;
	
	string temp = cals_are_here + "database.txt";
	dbase -> unload(temp.c_str());

	if (summary_only)
	  return (0);
      }
      
    }
    else {
      cerr << "Reading from database summary file" << endl;
      dbase = new Pulsar::Calibration::Database (cals_are_here.c_str());

      dbase -> test_inst(test_instr);
      dbase -> test_posn(test_coords);
      dbase -> test_time(test_times);
      dbase -> test_freq(test_frequency);
      dbase -> test_bw(test_bandwidth);
      dbase -> test_obst(test_obstype);
    }
  }
  catch (Error& error) {
    cerr << "Error generating CAL database" << endl;
    cerr << error << endl;
    cerr << "Aborting calibration attempt" << endl;
    return -1;
  }
  
  // Start calibrating archives
  
  for (unsigned i = 0; i < archives.size(); i++) {
    
    try {

      if (verbose)
	cerr << "Loading " << archives[i] << endl;

      Pulsar::Archive* arch = Pulsar::Archive::load(archives[i]);

      cout << "Loaded archive: " << archives[i] << endl;
      
     
      if (do_polncal) {

        if (verbose)
          cerr << "Finding PolnCalibrator" << endl;

	Pulsar::PolnCalibrator* pcal_engine  = 0;
	pcal_engine = dbase->generatePolnCalibrator(arch, m);
	
	if (display_params)	
	  pcal_engine->store_parameters = true;

        if (verbose)
          cerr << "Calibrating Archive Polarisation" << endl;

	pcal_engine->calibrate(arch);
	
	cout << "Polarisation Calibration Complete" << endl;
	
	if (display_params) {
	  cerr << "not implemented; use pacv" << endl;
	}	
      }

      /* The PolnCalibrator classes normalize everything so that flux
	 is given in units of the calibrator flux.  Unless the calibrator
	 is flux calibrated, it will undo the flux calibration step.
	 Therefore, the flux cal should take place after the poln cal */

      if (do_fluxcal) {

	Pulsar::FluxCalibrator* fcal_engine = 0;
	fcal_engine = dbase->generateFluxCalibrator(arch);

	if (verbose) {
	  cout << "Generated Flux Calibrator" << endl;
	  cout << "Calibrating Archive Fluxes" << endl;
	}

	fcal_engine->calibrate(arch);

	cout << "Flux calibration complete" << endl;

      }
 
      int index = archives[i].find_first_of(".", 0);
      string newname = archives[i].substr(0, index);
      newname += ".";
      newname += unload_ext;

      if (verbose)
        cerr << "pac: Calibrated Archive name '" << newname << "'" << endl;

      arch->unload(newname);
      
      cout << "New file " << newname << " unloaded" << endl;
      
    }
    catch (Error& error) {
      cerr << error << endl;
      exit(-1);
    }
  }
}




