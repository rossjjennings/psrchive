#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "dirutil.h"

#include "Pulsar/getopt.h"
#include "Pulsar/Integration.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FluxCalibrator.h"
#include "Calibration/Model.h"

#include "Error.h"

// Extensions this program understands

#include "Pulsar/ProcHistory.h"

// A command line tool for calibrating Pulsar::Archives
const char* args = "A:bcd:e:fFhiIn:op:PqsSt:Tu:vVwW";

void usage ()
{
  cout << "A program for calibrating Pulsar::Archives\n"
    "Usage: pac [options] filenames\n"
    "  -q                     Quiet mode\n"
    "  -v                     Verbose mode\n"
    "  -V                     Very verbose mode\n"
    "  -i                     Show revision information\n"
    "\n"
    "Database options: \n"
    "  -d database            Read ASCII summary (instead of -p) \n"   
    "  -p path                Search for CAL files in the specified path \n"
    "  -u \"ext1 ext2 ...\"     Add to file extensions recognized in search \n"
    "                         (defaults: .cf .pcal .fcal .pfit) \n"
    "  -w                     Write a new database summary file \n"
    "\n"
    "Calibrator options: \n"
    "  -A filename            Use the calibrator specified by filename \n"
    "  -P                     Calibrate polarisations only \n"
    "  -S                     Use the complete Reception model \n"
    "  -s                     Use the Polar Model \n"
    "\n"
    "Matching options: \n"
    "  -c                     Do not try to match sky coordinates\n"
    "  -I                     Do not try to match instruments\n"
    "  -T                     Do not try to match times\n"
    "  -F                     Do not try to match frequencies\n"
    "  -b                     Do not try to match bandwidths\n"
    "  -o                     Allow opposite sidebands\n"
    "\n"
    "Expert options: \n"
    "  -f                     Override flux calibration flag\n"
    "\n"
    "Output options: \n"
    "  -e extension           Use this extension when unloading results \n"
    "  -W                     do not correct profile weights \n"
    "  -n [q|u|v]             Flip the sign of Stokes Q, U, or V \n"
    "\n"
    "See http://astronomy.swin.edu.au/pulsar/software/manuals/pac.html"
       << endl;
}

#define Stokes_Q 0x01
#define Stokes_U 0x02
#define Stokes_V 0x04

void sign_flip (Pulsar::Archive* archive, unsigned ipol)
{
  for (unsigned isub=0; isub<archive->get_nsubint(); isub++)
    for (unsigned ichan=0; ichan<archive->get_nchan(); ichan++)
      *(archive->get_Profile (isub, ipol, ichan)) *= -1.0;
}

int main (int argc, char *argv[]) {
    
  bool verbose = false;
  bool new_database = true;
  bool do_fluxcal = true;
  bool do_polncal = true;

  bool write_database_file = false;
  bool check_flags = true;
  
  Pulsar::Calibrator::Type pcal_type = Pulsar::Calibrator::SingleAxis;

  // default searching criterion
  Pulsar::Database::Criterion criterion;

  string cals_are_here = "./";
  string unload_ext = "calib";
  string model_file;
  vector<string> exts;

  vector<string> archives;
  
  string pcal_file;

  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";

  string command = "pac ";

  unsigned char flip_sign = 0x00;

  while ((gotc = getopt(argc, argv, args)) != -1) 

    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'q':
      Pulsar::Archive::set_verbosity(0);
      break;

    case 'v':
      Pulsar::Archive::set_verbosity(2);
      Pulsar::Database::Criterion::match_verbose = true;
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Pulsar::Database::verbose = true;
      Pulsar::Calibrator::verbose = true;
      Pulsar::Database::Criterion::match_verbose = true;
      //Calibration::Model::verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'i':
      cout << "$Id: pac.C,v 1.58 2004/10/28 06:19:15 straten Exp $" << endl;
      return 0;

    case 'A':
      model_file = optarg;
      command += "-A ";
      break;

    case 'd':
      cals_are_here = optarg;
      new_database = false;
      break;

    case 'e':
      unload_ext = optarg;
      break;

    case 'f':
      check_flags = false;
      break;

    case 'n': {

      switch (optarg[0]) {

      case 'Q':
      case 'q':
	flip_sign |= Stokes_Q;
	break;

      case 'U':
      case 'u':
	flip_sign |= Stokes_U;
	break;

      case 'V':
      case 'v':
	flip_sign |= Stokes_V;
	break;

      }

      break;
    }

    case 'o':
      Pulsar::Archive::match_opposite_sideband = true;
      break;

    case 'p':
      cals_are_here = optarg;
      break;

    case 'P':
      do_fluxcal = false;
      command += "-P ";
      break;

    case 's':
      pcal_type = Pulsar::Calibrator::Polar;
      command += "-s ";
      break;

    case 'S':
      pcal_type = Pulsar::Calibrator::Hybrid;
      command += "-S ";
      break;

    case 'u':
      key = strtok (optarg, whitespace);
      while (key) {
	// remove the leading .
	while (*key == '.')
	  key ++;
        exts.push_back(key);
        key = strtok (NULL, whitespace);
      }
      break;

    case 'w':
      write_database_file = true;
      break;

    case 'W':
      Pulsar::PolnProfile::correct_weights = false;
      break;

    case 'b':
      criterion.check_bandwidth = false;
      command += "-b ";
      break;
    case 'c':
      criterion.check_coordinates = false;
      command += "-c ";
      break;
    case 'I':
      criterion.check_instrument = false;
      command += "-I ";
      break;
    case 'T':
      criterion.check_time = false;
      command += "-T ";
      break;
    case 'F':
      criterion.check_frequency = false;
      command += "-F ";
      break;

    default:
      return -1;
    }
 

  Pulsar::Database::set_default_criterion (criterion);

  for (int ai=optind; ai<argc; ai++)
    dirglob (&archives, argv[ai]);
  
  if (archives.empty()) {
    if (!write_database_file) {
      cout << "pac: No Archives were specified. Exiting" << endl;
      exit(-1);
    }
  } 

  // the archive from which a calibrator will be constructed
  Reference::To<Pulsar::Archive> model_arch;

  // the calibrator constructed from the specified archive
  Reference::To<Pulsar::PolnCalibrator> model_calibrator;

  // the database from which calibrators will be selected
  Pulsar::Database* dbase = 0;
  
  if ( !model_file.empty() ) try {

    cerr << "pac: Loading calibrator from " << model_file << endl;

    model_arch = Pulsar::Archive::load(model_file);
    model_calibrator = new Pulsar::PolnCalibrator(model_arch);
    pcal_type = model_calibrator->get_type();

  }
  catch (Error& error) {
    cerr << "pac: Could not load calibrator from " << model_file << endl;
    cerr << error << endl;
    return -1;
  }

  else if (new_database) try {
    
    // Generate the CAL file database
    
    exts.push_back("cf");
    exts.push_back("pcal");
    exts.push_back("fcal");
    exts.push_back("pfit");
    
    cout << "pac: Generating new calibrator database" << endl;
    
    dbase = new Pulsar::Database (cals_are_here, exts);
    
    if (dbase->size() <= 0) {
      cout << "pac: No calibrators found in " << cals_are_here << endl;
      return -1;
    }
    
    if (verbose)
      cerr << "pac: " << dbase->size() << " calibrators found" << endl;
    
    if (write_database_file) {
      
      string output_filename = dbase->get_path() + "/database.txt";
      
      cout << "pac: Writing database summary file to " 
	   << output_filename << endl;
      
      dbase -> unload (output_filename);
      
    }
    
  }
  catch (Error& error) {
    cerr << "pac: Error generating CAL database" << error << endl;
    return -1;
  }

  else try {
    
    cout << "pac: Reading from database summary file" << endl;
    dbase = new Pulsar::Database (cals_are_here);

  }
  catch (Error& error) {
    cerr << "pac: Error loading CAL database" << error << endl;
    return -1;
  }
    
  // Start calibrating archives
  
  for (unsigned i = 0; i < archives.size(); i++) try {

    cout << endl;

    if (verbose)
      cerr << "pac: Loading " << archives[i] << endl;
    
    Reference::To<Pulsar::Archive> arch = Pulsar::Archive::load(archives[i]);
    
    cout << "pac: Loaded archive " << archives[i] << endl;
    
    bool successful_polncal = false;

    if (do_polncal && arch->get_poln_calibrated() )
      cout << "pac: " << archives[i] << " already poln calibrated" << endl;

    else if (do_polncal && !arch->get_poln_calibrated()) {
      
      Reference::To<Pulsar::PolnCalibrator> pcal_engine;

      if (model_calibrator) {

	if (verbose)
	  cout << "pac: Applying specified calibrator" << endl;

	pcal_engine = model_calibrator;

      }
      else {

	if (verbose)
	  cout << "pac: Finding PolnCalibrator" << endl;
	  
	pcal_engine = dbase->generatePolnCalibrator(arch, pcal_type);
	
      }
      
      pcal_file = pcal_engine->get_filenames();

      cout << "pac: PolnCalibrator constructed from:\n\t" << pcal_file << endl;

      pcal_engine->calibrate (arch);

      if (arch->get_npol() == 4) {
	if (verbose)
	  cerr << "pac: Correcting platform" << endl;

	arch->correct_instrument ();
      }

      cout << "pac: Poln calibration complete" << endl;

      successful_polncal = true;

    }
        
    /* The PolnCalibrator classes normalize everything so that flux
       is given in units of the calibrator flux.  Unless the calibrator
       is flux calibrated, it will undo the flux calibration step.
       Therefore, the flux cal should take place after the poln cal */

    bool successful_fluxcal = false;
    
    if (do_fluxcal && arch->get_scale() == Signal::Jansky && check_flags)
      cout << "pac: " << archives[i] << " already flux calibrated" << endl;
    
    else if (!dbase)
      cout << "pac: Not performing flux calibration (no database)." << endl;

    else if (do_fluxcal) try {

      if (verbose)
	cout << "pac: Generating flux calibrator" << endl;
      
      Reference::To<Pulsar::FluxCalibrator> fcal_engine;
      fcal_engine = dbase->generateFluxCalibrator(arch);
      
      cout << "pac: FluxCalibrator constructed from:\n\t"
	   << fcal_engine->get_filenames() << endl;

      if (verbose) 
	cerr << "pac: Calibrating Archive fluxes" << endl;
      
      fcal_engine->calibrate(arch);
      
      cout << "pac: Flux calibration complete" << endl;
      
      successful_fluxcal = true;
      
      cout << "pac: Mean Tsys = " << fcal_engine->meanTsys() 
	   << " mJy" << endl;
      
    }
    catch (Error& error) {
      cerr << "pac: Could not flux calibrate " << arch->get_filename() << endl
	   << "\t" << error.get_message() << endl;
    }

    // find first of "." turns ./cal/poo.cfb info .unload_ext WvS
    //
    // int index = archives[i].find_first_of(".", 0);

    unsigned index = archives[i].find_last_of(".",archives[i].length());

    if (index == string::npos)
      index = archives[i].length();

    // starting the output filename with the cwd causes a mess when 
    // full path names to input files are used.  WvS
    //  
    // string newname = opath;
    
    string newname = archives[i].substr(0, index);
    
    newname += ".";
    newname += unload_ext;

    if (!successful_fluxcal)
      newname += "P";

    if (verbose)
      cerr << "pac: Calibrated Archive name '" << newname << "'" << endl;
    

    if (flip_sign) {

      cerr << "pac: Flipping the sign of Stokes";
      arch->convert_state (Signal::Stokes);

      if (flip_sign & Stokes_Q) {
	cerr << " Q";
	sign_flip (arch, 1);
      }

      if (flip_sign & Stokes_U) {
	cerr << " U";
	sign_flip (arch, 2);
      }

      if (flip_sign & Stokes_V) {
	cerr << " V";
	sign_flip (arch, 3);
      }

      cerr << endl;

    }

    // See if the archive contains a history that should be updated
    
    Pulsar::ProcHistory* fitsext = arch->get<Pulsar::ProcHistory>();
    
    if (fitsext) {
      
      if (successful_polncal) {
	
	switch (pcal_type) {
	  
	case Pulsar::Calibrator::SingleAxis:
	  fitsext->set_cal_mthd("SingleAxis");
	  break;
	  
	case Pulsar::Calibrator::Polar:
	  fitsext->set_cal_mthd("Polar");
	  break;
	  
	case Pulsar::Calibrator::Britton:
	case Pulsar::Calibrator::Hybrid:
	  fitsext->set_cal_mthd("FullCAL");
	  break;
	  
	default:
	  fitsext->set_cal_mthd("unknown");
	  break;
	  
	}
	
	fitsext->set_cal_file(pcal_file);
      }
      
      if (command.length() > 80) {
	cout << "pac: WARNING: ProcHistory command string truncated to 80 chars" << endl;
	fitsext->set_command_str(command.substr(0, 80));
      }
      else {
	fitsext->set_command_str(command);
      }
    }

    if (verbose)
      cerr << "pac: Unloading " << newname << endl;
    
    arch->unload(newname);
    
    cout << "pac: Calibrated archive " << newname << " unloaded" << endl;
    
  }
  catch (Error& error) {
    cerr << "pac: Error while handling " << archives[i] << ":" << endl; 
    cerr << error << endl;
  }

  cout << "\npac: Finished all files" << endl;
  return 0;
}




