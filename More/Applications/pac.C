/***************************************************************************
 *
 *   Copyright (C) 2003-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#if 1
#include "Pulsar/psrchive.h"
#include "Pulsar/Interpreter.h"
#else
#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"
#endif

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/PolnProfile.h"

#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/ManualPolnCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/Receiver.h"

#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/BackendCorrection.h"
#include "Pulsar/FrontendCorrection.h"
#include "Pulsar/ProjectionCorrection.h"
#include "Pulsar/ReflectStokes.h"
#include "Pulsar/BackendFeed.h"
#include "Pulsar/VariableBackend.h"

#include "Pulsar/ProcHistory.h"
#include "Pulsar/Feed.h"

#include "Error.h"
#include "dirutil.h"
#include "strutil.h"

#include <unistd.h>
#include <string.h>

using namespace std;
using namespace Pulsar;

// A command line tool for calibrating Pulsar::Archives
const char* args = "A:aBbC:cDd:Ee:fFGghiIJ:j:K:k:lLM:m:Nn:O:op:PqQ:Rr:sSt:Tu:UvVwWxXyY:Z";

void usage ()
{
  cout << "A program for calibrating Pulsar::Archives\n"
    "Usage: pac [options] filenames\n"
    "  -q             Quiet mode\n"
    "  -v             Verbose mode\n"
    "  -V             Very verbose mode\n"
    "  -i             Show revision information\n"
    "\n"
    "Database options: \n"
    "  -d database    Read calibration database summary \n"   
    "  -p path        Search for CAL files in the specified path \n"
    "  -u ext         Add to file extensions recognized in search \n"
    "                 (defaults: .cf .pcal .fcal .pfit) \n"
    "  -w             Write a new database summary file \n"
    "  -W             Create database from calibrators listed in metafiles \n"
    "  -k filename    Output database to filename \n"
    "  -l             Cache last calibrator\n"
    "\n"
    "Calibrator options: \n"
    "  -A filename    Use the calibrator in filename, as output by pcm/pacv \n"
    "  -Q filename    Use the Jones calibrator in filename \n"
    "  -Y filename    Load projection transformations from file \n"
    "  -C model       Use the specified model of the calibrator solution \n"
    "  -P             Calibrate polarisation only (not flux)\n"
    "  -R             Calibrate the receiver (feed) only \n"
    "  -r filename    Use the specified receiver parameters file \n"
    "  -S             Use the complete Reception model \n"
    "  -s             Use the Polar Model \n"
    "  -I             Correct ionospheric Faraday rotation using IRI\n"
    "  -x             Derive calibrator Stokes parameters from fluxcal data\n"
    "  -y             Always trust the Pointing::feed_angle attribute \n"
    "  -g             Fscrunch data to match number of channels of calibrator\n"
    "  -K sigma       Reject outliers when computing CAL levels \n"
    "\n"
    "Rough Alignment options [not recommended]: \n"
    "  -B             Fix the off-pulse baseline statistics \n"
    "  -D             Fix the reference degree of polarization \n"
    "\n"
    "Matching options: \n"
    "  -m [b|a]       Use only calibrator before|after observation\n"
    "  -T             Take closest time (no maximum interval)\n"
    "  -c             Take closest sky coordinates (no maximum distance)\n"
    "  -Z             Do not try to match instruments\n"
    "  -F             Do not try to match frequencies\n"
    "  -b             Do not try to match bandwidths\n"
    "  -o             Allow opposite sidebands\n"
    "  -a             Per-channel matching\n"
    "  -L             Print verbose matching information \n"
    "\n"
    "Expert options: \n"
    "  -f             Override flux calibration flag\n"
    "  -G             Normalize profile weights by absolute gain \n"
    "  -N             Disable backend corrections \n"
    "  -U             Disable frontend corrections (parallactic angle, etc)\n"
    "  -X             Disable poln calibration \n"
    "\n"
    "Input/Output options: \n"
    "  -e ext         Extension added to output filenames (default .calib) \n"
    "  -j job1[,jobN] Preprocessing job[s] \n"
    "  -J jobs        Multiple preprocessing jobs in 'jobs' file \n"
    "  -M meta        File from which input filenames are read \n"
    "  -n [q|u|v]     Flip the sign of Stokes Q, U, or V \n"
    "  -O path        Path to which output files are written \n"
    "\n"
    "See " PSRCHIVE_HTTP "/manuals/pac for more details\n"
       << endl;
}

// cut down the calibrator solution to only the feed
void keep_only_feed( Pulsar::PolnCalibrator* );

int main (int argc, char *argv[]) try
{    
  bool verbose = false;
  bool do_fluxcal = true;
  bool do_polncal = true;
  bool do_backend = true;
  bool do_frontend = true;

  bool use_fluxcal_stokes = false;
  bool fscrunch_data_to_cal = false;
  float outlier_threshold = 0.0;

  // Flag for only displaying the system-equivalent flux density.
  bool only_display_sefd = false;

  // Preprocessing jobs
  vector<string> jobs;

  bool write_database_file = false;
  bool check_flags = true;
  // By default, don't use last calibrator caching
  Pulsar::Database::cache_last_cal = false;

  // name of the file containing the list of Archive filenames
  char* metafile = NULL;

  // treat all files as metafiles containing lists of calibrator filenames
  bool cals_metafile = false;

  // known feed transformation
  Calibration::Feed* feed = 0;

  // model ionosphere
  Pulsar::IonosphereCalibrator* ionosphere = 0;

  // default calibrator type
  Reference::To<const Calibrator::Type> pcal_type;
  pcal_type = new CalibratorTypes::SingleAxis;

  // default searching criteria
  Database::Criteria criteria;

  string cals_are_here = "./";

  string database_filename;
  vector<string> cal_dbase_filenames;

  // directory to which calibrated output files are written
  string unload_path;
  string unload_ext = "calib";

  // filename from which calibrator solution to be applied will be loaded
  string model_file;
  string ascii_model_file;
  string projection_file;
  
  // strip down the above-named calibrator solution to keep only the feed
  bool apply_only_feed = false;

  vector<string> exts;

  string pcal_file;

  int gotc = 0;
  char* key = NULL;
  char whitespace[5] = " \n\t";

  string command = "pac ";

  string optarg_str;

  ReflectStokes reflections;

  while ((gotc = getopt(argc, argv, args)) != -1) 

    switch (gotc) {

    case 'h':
      usage ();
      return 0;

    case 'q':
      Archive::set_verbosity(0);
      break;

    case 'v':
      Archive::set_verbosity(2);
      verbose = true;
      break;

    case 'V':
      verbose = true;
      Archive::set_verbosity(3);
      break;

    case 'i':
      cout << "$Id: pac.C,v 1.109 2011/02/17 07:43:53 straten Exp $" << endl;
      return 0;

    case 'k':
      database_filename = optarg;
      break;

    case 'K':
      outlier_threshold = atof(optarg);
      break;

    case 'A':
      model_file = optarg;
      command += " -A " + basename(model_file);
      break;

    case 'B':
      pcal_type = new CalibratorTypes::OffPulse;
      command += " -B";
      break;

    case 'C':
      pcal_type = Calibrator::Type::factory(optarg);
      command += " -C ";
      command += optarg;
      break;

    case 'D':
      pcal_type = new CalibratorTypes::DoP;
      command += " -D";
      break;

    case 'd':
      cal_dbase_filenames.push_back (optarg);
      command += " -d " + basename (string(optarg));
      break;

    case 'e':
      unload_ext = optarg;
      command += " -e ";
      command += optarg;
      break;

    case 'E':
      only_display_sefd = true;
      do_polncal = false;
      break;

    case 'f':
      check_flags = false;
      command += " -f";
      break;
    
    case 'g':
      fscrunch_data_to_cal = true;
      break;

    case 'G':
      PolnProfile::normalize_weight_by_absolute_gain = true;
      command += " -g";
      break;

    case 'I':
      ionosphere = new IonosphereCalibrator;
      break;

    case 'j':
      separate (optarg, jobs, ",");
      break;

    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'l':
      Database::cache_last_cal = true;
      command += " -l";
      break;

    case 'L':
      Database::match_verbose = true;
      break;

    case 'Q':
      ascii_model_file = optarg;
      command += " -Q " + basename(ascii_model_file);
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'm': 
      if (optarg[0] == 'b')
        criteria.set_sequence(Database::CalibratorBefore);
      else if (optarg[0] == 'a')
        criteria.set_sequence(Database::CalibratorAfter);
      else {
        cerr << "pac: unrecognized matching sequence code" << endl;
        return -1;
      }
      command += " -m ";
      command += optarg;
      break;

    case 'N':
      do_backend = false;
      command += " -N ";
      break;

    case 'n': {

      reflections.add_reflection( optarg[0] );

      command += " -n ";
      command += optarg;
      break;

    }

    case 'O':
      unload_path = optarg;
      command += " -O ";
      command += optarg;
      break;

    case 'o':
      Archive::Match::opposite_sideband = true;
      command += " -o";
      break;

    case 'p':
      cals_are_here = optarg;
      command += " -p " + basename(cals_are_here);
      break;

    case 'P':
      do_fluxcal = false;
      command += " -P";
      break;

    case 'R':
      apply_only_feed = true;
      command += " -R";
      break;

    case 'r':
      feed = new Calibration::Feed;
      feed -> load (optarg);
      cerr << "pac: Feed parameters loaded:"
        "\n  orientation 0 = "
           << feed->get_orientation(0).get_value() * 180/M_PI << " deg"
        "\n  ellipticity 0 = "
           << feed->get_ellipticity(0).get_value() * 180/M_PI << " deg"
        "\n  orientation 1 = "
           << feed->get_orientation(1).get_value() * 180/M_PI << " deg"
        "\n  ellipticity 1 = "
           << feed->get_ellipticity(1).get_value() * 180/M_PI << " deg"
           << endl;
      command += " -r " + basename(string(optarg));
      break;

    case 's':
      pcal_type = new CalibratorTypes::van02_EqA1;
      command += " -s";
      break;

    case 'S':
      pcal_type = new CalibratorTypes::ovhb04;
      command += " -S";
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
      command += " -u ";
      command += optarg;
      break;

    case 'U':
      do_frontend = false;
      command += " -U ";
      break;

    case 'w':
      write_database_file = true;
      command += " -w";
      break;

    case 'W':
      write_database_file = true;
      cals_metafile = true;
      command += " -W";
      break;

    case 'x':
      use_fluxcal_stokes = true;
      command += " -x";
      break;

    case 'X':
      do_polncal = false;               // disable poln calibration
      unload_ext = "bc";                // "basis corrected"
      break;

    case 'Y':
      projection_file = optarg;
      command += " -Y " + basename(projection_file);
      do_frontend = false;
      break;

    case 'y':
      ProjectionCorrection::trust_pointing_feed_angle = true;
      command += " -y";
      break;

    case 'b':
      criteria.check_bandwidth = false;
      command += " -b";
      break;
    case 'c':
      criteria.check_coordinates = false;
      command += " -c";
      break;
    case 'T':
      criteria.check_time = false;
      command += " -T";
      break;
    case 'F':
      criteria.check_frequency = false;
      command += " -F";
      break;
    case 'Z':
      criteria.check_instrument = false;
      command += " -Z";
      break;
    case 'a':
      criteria.check_frequency = false;
      criteria.check_bandwidth = false;
      criteria.check_frequency_array = true;
      break;

    default:
      return -1;
    }


  Database::set_default_criteria (criteria);

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty())
  {
    if (!write_database_file || cals_metafile)
    {
      cout << "pac: No filenames specified. Exiting" << endl;
      exit(-1);
    }
  }

  // the archive from which a calibrator will be constructed
  Reference::To<Archive> model_arch;

  // the calibrator constructed from the specified archive
  Reference::To<PolnCalibrator> model_calibrator;

  // the database from which calibrators will be selected
  Reference::To<Database> dbase;

  if ( !model_file.empty() ) try
  {
    cerr << "pac: Loading calibrator from " << model_file << endl;

    if (criteria.check_frequency_array)
      cerr << "pac: Warning: -a and -A options are incompatible" << endl;

    model_arch = Archive::load(model_file);

    if (model_arch->get<PolnCalibratorExtension>())
      model_calibrator = new PolnCalibrator(model_arch);
    else
      model_calibrator = ReferenceCalibrator::factory(pcal_type, model_arch);

    if (apply_only_feed)
      keep_only_feed (model_calibrator);

    pcal_type = model_calibrator->get_type();
  }
  catch (Error& error)
  {
    cerr << "pac: Could not load calibrator from " << model_file << endl;
    cerr << error << endl;
    return -1;
  }

  // Manual calibration
  if ( !ascii_model_file.empty() ) try
  {
    cerr << "pac: Loading ascii Jones calibrator from " << ascii_model_file << endl;
    model_calibrator = new ManualPolnCalibrator(ascii_model_file);
    pcal_type = model_calibrator->get_type();
  }
  catch (Error& error)
  {
    cerr << "pac: Could not load calibrator from " << ascii_model_file << endl;
    cerr << error << endl;
    return -1;
  }

  if ( use_fluxcal_stokes && 
       ! pcal_type->is_a<CalibratorTypes::SingleAxis>() )
  {
    cerr << "pac: Fluxcal-derived Stokes params are incompatible with the "
      << "selected calibration method" << endl;
    return -1;
  }

  if (cal_dbase_filenames.size()) try
  {
    for (unsigned i=0; i<cal_dbase_filenames.size(); i++)
    {
      cout << "pac: Loading database from " << cal_dbase_filenames[i] << endl;
      Reference::To<Database> tmp = new Database (cal_dbase_filenames[i]);
      if (!dbase)
        dbase = tmp;
      else
        dbase->merge(tmp);
    }
  }
  catch (Error& error)
  {
    cerr << "pac: Error loading CAL database" << error << endl;
    return -1;
  }

  else if ( model_file.empty() && ascii_model_file.empty() && (do_polncal || do_fluxcal) ) try
  {
    // Generate the CAL file database
    cout << "pac: Generating new calibrator database" << endl;

    if (cals_metafile)
    {
      Reference::To<Database> temp;

      for (unsigned i=0; i < filenames.size(); i++)
      {
        cout << "pac: Loading calibrator filenames from metafile="
             << filenames[i] << endl;

        temp = new Database (cals_are_here, filenames[i]);

        if (temp->size() <= 0)
        {
          cerr << "pac: No calibrators found in " << cals_are_here
               << " listed in " << filenames[i] << endl;
          continue;
        }

        if (!dbase)
          dbase = temp;
        else
          dbase -> merge (temp);
      }

      if (dbase->size() <= 0)
      {
        cerr << "pac: No calibrators found in the provided metafiles" << endl;
        return -1;
      }
    }
    else
    {
      exts.push_back("cf");
      exts.push_back("pcal");
      exts.push_back("fcal");
      exts.push_back("pfit");

      dbase = new Database (cals_are_here, exts);

      if (dbase->size() <= 0)
      {
        cerr << "pac: No calibrators found in " << cals_are_here << endl;
        return -1;
      }
    }

    if (verbose)
      cerr << "pac: " << dbase->size() << " calibrators found" << endl;

    if (write_database_file)
    {
      if (database_filename.empty ())
        database_filename = dbase->get_path() + "/database.txt";

      cout << "pac: Writing database summary file to "
           << database_filename << endl;

      dbase -> unload (database_filename);
    }

    // all of the supplied filenames are treated as metafiles
    if (cals_metafile)
      return 0;
  }
  catch (Error& error)
  {
    cerr << "pac: Error generating CAL database" << error << endl;
    return -1;
  }

  if (feed)
    dbase -> set_feed (feed);

  // Start calibrating archives
  
  Interpreter* preprocessor = standard_shell();

  for (unsigned i = 0; i < filenames.size(); i++) try
  {
    cout << endl;

    if (verbose)
      cerr << "pac: Loading " << filenames[i] << endl;

    Reference::To<Archive> arch = Archive::load(filenames[i]);

    cout << "pac: Loaded archive " << filenames[i] << endl;

    if (jobs.size())
    {
      if (verbose)
        cerr << "pac: preprocessing " << filenames[i] << endl;
      preprocessor->set (arch);
      preprocessor->script (jobs);
    }

    if (do_backend && (arch->get_npol() == 4 || arch->get_npol() == 2))
    {
      if (verbose)
        cerr << "pac: Correcting backend, if necessary" << endl;

      BackendCorrection correct;
      correct (arch);
    }
    else
      cerr << "pac: Backend corrections disabled" << endl;

    bool successful_polncal = false;

    if (do_polncal && arch->get_poln_calibrated() )
    {
      cout << "pac: " << filenames[i] << " already poln calibrated" << endl;
      successful_polncal = true;
    }

    else if (do_polncal && !arch->get_poln_calibrated())
    {
      Reference::To<PolnCalibrator> pcal_engine;

      if (model_calibrator)
      {
        if (verbose)
          cout << "pac: Applying specified calibrator" << endl;

        pcal_engine = model_calibrator;
      }

      else try
      {
        if (verbose)
          cout << "pac: Finding PolnCalibrator" << endl;

        pcal_engine = dbase->generatePolnCalibrator(arch, pcal_type);
      }
      catch (Error& error)
      {
        error << " -- closest match: \n\n"
              << dbase->get_closest_match_report ();
        throw error;
      }

      if (use_fluxcal_stokes) try
      {
        if (verbose)
          cout << "pac: Calculating fluxcal Stokes params" << endl;

        ReferenceCalibrator* refcal = 0;
        refcal = dynamic_cast<ReferenceCalibrator*> (pcal_engine.get());
        if (!refcal)
          throw Error (InvalidState, "pcm",
                       "PolnCalibrator is not a ReferenceCalibrator");

        // Find appropriate fluxcal from DB 
        Reference::To<FluxCalibrator> flux_cal;
        try
        {
          flux_cal = dbase->generateFluxCalibrator(arch);
        }
        catch (Error& error)
        {
          error << " -- closest match: \n\n"
                << dbase->get_closest_match_report ();
          throw error;
        }

        // Combine already-selected pcal_engine with fluxcal stokes
        // into a new HybridCalibrator
        Reference::To<HybridCalibrator> hybrid_cal;
        hybrid_cal = new HybridCalibrator;
        hybrid_cal->set_reference_input( flux_cal->get_CalibratorStokes(),
                                         flux_cal->get_filenames() );

        hybrid_cal->set_reference_observation( refcal );

        pcal_engine = hybrid_cal;
      }
      catch (Error& error)
      {
        cerr << "pac: Error computing cal Stokes for " << arch->get_filename()
          << endl
          << "\t" << error.get_message() << endl;
      }

      pcal_file = pcal_engine->get_filenames();

      cout << "pac: PolnCalibrator constructed from:\n\t" << pcal_file << endl;

      if (fscrunch_data_to_cal && pcal_engine->get_nchan() != arch->get_nchan())
      {
        cout << "pac: Frequency integrating data (nchan=" << arch->get_nchan()
             << ") to match calibrator (nchan=" << pcal_engine->get_nchan()
             << ")" << endl;
        arch->fscrunch_to_nchan (pcal_engine->get_nchan());
      }

      if (outlier_threshold)
      {
        ReferenceCalibrator* ref = 0;
        ref = dynamic_cast<ReferenceCalibrator*> (pcal_engine.get());
        if (ref)
          ref->set_outlier_threshold (outlier_threshold);

        HybridCalibrator* hyb = 0;
        hyb = dynamic_cast<HybridCalibrator*> (pcal_engine.get());
        if (hyb)
        {
          ref = const_cast<ReferenceCalibrator*>(hyb->get_reference_observation ());
          ref->set_outlier_threshold (outlier_threshold);
        }
      }

      pcal_engine->set_backend_correction( do_backend );
      pcal_engine->calibrate (arch);

      cout << "pac: Poln calibration complete" << endl;

      successful_polncal = true;
    }
    else
    {
      cerr << "pac: Poln calibration disabled" << endl;
    }

    if (do_frontend && (arch->get_npol() == 4))
    {
      if (verbose)
        cerr << "pac: Correcting fronted, if necessary" << endl;

      FrontendCorrection correct;
      correct.calibrate (arch);
    }
    else
      cerr << "pac: Frontend corrections disabled." << endl;

    if ( ! projection_file.empty() )
    {
      cerr << "pac: Loading projection transformations from "
	   << projection_file << endl;

      Receiver* receiver = arch->get<Receiver>();

      BasisCorrection basis_correction;
      if ( basis_correction.required (arch) )
      {
	arch->transform( inv( basis_correction(arch) ) );
	receiver->set_basis_corrected (true);
      }
      
      Reference::To<ManualPolnCalibrator> calibrator;
      calibrator = new ManualPolnCalibrator (projection_file);
      calibrator->calibrate (arch);

      receiver->set_projection_corrected (true);
    }
    
    if (ionosphere)
    {
      cerr << "pac: Correcting ionospheric Faraday rotation" << endl;
      ionosphere->calibrate (arch);
    }

    /* The PolnCalibrator classes normalize everything so that flux
       is given in units of the calibrator flux. Unless the calibrator
       is flux calibrated, it will undo the flux calibration step.
       Therefore, the flux cal should take place after the poln cal */

    bool successful_fluxcal = false;

    if (do_fluxcal && arch->get_scale() == Signal::Jansky && check_flags)
      cout << "pac: " << filenames[i] << " already flux calibrated" << endl;

    else if (do_fluxcal && !dbase)
      cout << "pac: Not performing flux calibration (no database)." << endl;

    else if (do_fluxcal) try
    {
      if (verbose)
        cout << "pac: Generating flux calibrator" << endl;

      Reference::To<FluxCalibrator> fcal_engine;

      try
      {
        fcal_engine = dbase->generateFluxCalibrator(arch);
      }
      catch (Error& error)
      {
        error << " -- closest match: \n\n"
              << dbase->get_closest_match_report ();
        throw error;
      }

      cout << "pac: Mean SEFD = " << fcal_engine->meanTsys() * 1e-3
        << " Jy" << endl;

      if (only_display_sefd)
      {
        // Skip the flux-calibration, HISTORY table modification, and unloading 
        // of a new file.
        continue;
      }

      cout << "pac: FluxCalibrator constructed from:\n\t"
           << fcal_engine->get_filenames() << endl;

      fcal_engine->set_outlier_threshold (outlier_threshold);

      if (verbose) 
        cerr << "pac: Calibrating Archive fluxes" << endl;

      fcal_engine->calibrate(arch);

      cout << "pac: Flux calibration complete" << endl;

      successful_fluxcal = true;
    }
    catch (Error& error)
    {
      cerr << "pac: Could not flux calibrate " << arch->get_filename() << endl
           << "\t" << error << endl;
    }

    string newname = replace_extension( filenames[i], unload_ext );

    if (successful_polncal && !successful_fluxcal)
      newname += "P";

    if (!unload_path.empty())
      newname = unload_path + "/" + basename (newname);

    if (verbose)
      cerr << "pac: Calibrated Archive name '" << newname << "'" << endl;

    reflections.transform (arch);

    // See if the archive contains a history that should be updated

    ProcHistory* fitsext = arch->get<ProcHistory>();

    if (fitsext)
    {
      fitsext->set_command_str(command);
    }

    if (verbose)
      cerr << "pac: Unloading " << newname << endl;

    arch->unload(newname);

    cout << "pac: Calibrated archive " << newname << " unloaded" << endl;

  }
  catch (Error& error)
  {
    cerr << "pac: Error while handling " << filenames[i] << ":" << endl;
    cerr << error << endl;
  }

  cout << "\npac: Finished all files" << endl;
  return 0;

}
 catch (Error& error)
 {
   cerr << "pac: error" << error << endl;
   return -1;
 }

using Calibration::BackendFeed;

void keep_only_feed( PolnCalibrator* cal )
{
  const unsigned nchan = cal->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!cal->get_transformation_valid(ichan))
      continue;

    MEAL::Complex2* xform = cal->get_transformation(ichan);

    BackendFeed* feed = dynamic_cast<BackendFeed*> (xform);

    if (!feed)
      throw Error (InvalidState, "keep_only_feed",
                   "PolnCalibrator at ichan=%u is not a "
                   "BackendFeed transformation", ichan);

    feed->get_backend()->set_gain( 1.0 );
    feed->get_backend()->set_diff_gain( 0.0 );
    feed->get_backend()->set_diff_phase( 0.0 );
  }
}
