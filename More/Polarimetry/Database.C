#include "Pulsar/Database.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/FluxCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "Pulsar/Archive.h"

#include "Error.h"

#include "Stokes.h"
#include "Jones.h"
#include "MJD.h"
#include "sky_coord.h"

#include "dirutil.h"
#include <unistd.h> 
#include <errno.h>

bool Pulsar::Database::verbose = false;

/*! By default, the long time scale is set to four weeks. */
double Pulsar::Database::long_time_scale = 60.0 * 24.0 * 28.0;

/*! By default, the short time scale is set to one hour. */
double Pulsar::Database::short_time_scale = 60.0;


// //////////////////////////////////////////////////////////////////////
//
// Pulsar::Database::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Pulsar::Database::Entry::init ()
{
  obsType = Signal::Unknown;
  calType = Calibrator::SingleAxis;

  time.Construct("00000.00000"); 
  
  sky_coord temp("00:00:00.0 00:00:00");
  position = temp;
  
  bandwidth = 0.0;
  frequency = 0.0; 
  nchan = 0; 
  
  receiver = "unset";
  instrument = "unset";
  filename = "unset";
}

//! Construct from a Pulsar::Archive
Pulsar::Database::Entry::Entry (const Pulsar::Archive& arch)
{
  obsType = arch.get_type();

  const Pulsar::Backend* backend = arch.get<Backend>();
  if (!backend)
    throw Error (InvalidState, "Pulsar::Database::Entry",
		 "Archive has no Backend Extension");

  const Pulsar::Receiver* receiver_ext = arch.get<Receiver>();
  if (!receiver_ext)
    throw Error (InvalidState, "Pulsar::Database::Entry",
		 "Archive has no Receiver Extension");

  if (obsType == Signal::Calibrator) {

    const CalibratorExtension* ext = arch.get<CalibratorExtension>();

    if (!ext)
      throw Error (InvalidState, "Pulsar::Database::Entry",
		   "Archive::get_type==Signal::Calibrator but"
		   " no CalibratorExtension");

    calType = ext->get_type();
    time = ext->get_epoch();

  }
  else
    time = ( arch.start_time() + arch.end_time() ) / 2.0;

  position = arch.get_coordinates();
  bandwidth = arch.get_bandwidth();
  frequency = arch.get_centre_frequency();
  nchan = arch.get_nchan();
  filename = arch.get_filename();

  instrument = backend->get_name();
  receiver = receiver_ext->get_name();
}

//! Destructor
Pulsar::Database::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::Database::Entry::load (const char* str) 
{
  const char* whitespace = " \t\n";
  string line = str;

  // /////////////////////////////////////////////////////////////////
  // filename
  filename = stringtok (&line, whitespace);
  
  // /////////////////////////////////////////////////////////////////
  // type
  string typestr = stringtok (&line, whitespace);

  obsType = Signal::string2Source(typestr);
  if (obsType == Signal::Unknown) {
    obsType = Signal::Calibrator;
    calType = Calibrator::str2Type (typestr.c_str());
  }

  // /////////////////////////////////////////////////////////////////
  // RA DEC
  string coordstr = stringtok (&line, whitespace);
  coordstr += " " + stringtok (&line, whitespace);
  position = sky_coord (coordstr.c_str());

  // /////////////////////////////////////////////////////////////////
  // MJD
  string mjdstr  = stringtok (&line, whitespace);
  time = MJD (mjdstr);

  // /////////////////////////////////////////////////////////////////
  // bandwidth, frequency, number of channels
  int s = sscanf (line.c_str(), "%lf %lf %d", &bandwidth, &frequency, &nchan);
  if (s != 3)
    throw Error (FailedSys, "Pulsar::Database::Entry::load",
                 "sscanf(" + line + ") != 3");

  stringtok (&line, whitespace);
  stringtok (&line, whitespace);
  stringtok (&line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // instrument
  instrument = stringtok (&line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // receiver
  receiver = stringtok (&line, whitespace);

  if (receiver.length() == 0)
    throw Error (InvalidParam, "Pulsar::Database::Entry::load",
                 "Could not parse '%s'");

}

// unload to a string
void Pulsar::Database::Entry::unload (string& retval)
{
  retval = filename + " ";
  
  if (obsType == Signal::Calibrator)
    retval += Calibrator::Type2str (calType);
  else
    retval += Signal::Source2string (obsType);

  retval += " ";
  retval += position.getRaDec().getHMSDMS();

  retval += " ";
  retval += time.printdays(15);

  retval += stringprintf (" %lf %lf %d", bandwidth, frequency, nchan);

  retval += " " + instrument + " " + receiver;
}

bool Pulsar::Database::Criterion::match_verbose = false;

Pulsar::Database::Criterion::Criterion ()
{
  minutes_apart = 720.0;
  RA_deg_apart  = 5.0;
  DEC_deg_apart = 5.0;

  check_receiver    = true;
  check_instrument  = true;
  check_frequency   = true;
  check_bandwidth   = true;
  check_obs_type    = true;
  check_time        = true;
  check_coordinates = true;
}


//! returns true if this matches observation parameters
bool Pulsar::Database::Criterion::match (const Entry& have) const
{

  if (match_verbose)
    cerr << "Pulsar::Database::Criterion::match" << endl;
 
  if (check_obs_type) {

    if (match_verbose)
      cerr << "  Seeking obsType="
	   << Signal::source_string(entry.obsType) 
	   << " have obsType="
	   << Signal::source_string(have.obsType);
    
    if (entry.obsType == have.obsType) {
      if (match_verbose)
	cerr << "... match found" << endl; 
    }
    else {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }

    if (entry.obsType == Signal::Calibrator) {

      if (match_verbose)
	cerr << "  Seeking calType="
	     << Calibrator::Type2str(entry.calType) 
	     << " have calType="
	     << Calibrator::Type2str(have.calType);

      if (entry.calType == have.calType) {
	if (match_verbose)
	  cerr << "... match found" << endl; 
      }
      else {
	if (match_verbose)
	  cerr << "... no match" << endl;
	return false;
      }

    }

  }

  if (check_receiver) {

    if (match_verbose)
      cerr << "  Seeking receiver=" << entry.receiver
           << " have receiver=" << have.receiver;

    if (entry.receiver==have.receiver) {
      if (match_verbose)
        cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_instrument) {

    if (match_verbose)
      cerr << "  Seeking instrument=" << entry.instrument
           << " have instrument=" << have.instrument;

    if (entry.instrument==have.instrument) {
      if (match_verbose)
        cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_frequency) {

    if (match_verbose)
      cerr << "  Seeking frequency=" << entry.frequency
	   << " have frequency=" << have.frequency;

    if (entry.frequency==have.frequency) {
      if (match_verbose)
	cerr << " ... match found" << endl; 
    }
    else {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }
  }

  if (check_bandwidth) {

    if (match_verbose)
      cerr << "  Seeking bandwidth=" << entry.bandwidth
           << " have bandwidth=" << have.bandwidth;

    if (entry.bandwidth==have.bandwidth) {
      if (match_verbose)
        cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
        cerr << "... no match" << endl;
      return false;
    }
  }

  double diff;

  if (check_time) {

    diff = fabs( (have.time - entry.time).in_minutes() );

    if (match_verbose) {
      cerr << "  Seeking time=" << entry.time
	   << " have time=" << have.time
	   << "\n    difference=" << diff << " minutes "
	"(max=" << minutes_apart << ")";
    }

    if (fabs( (have.time - entry.time).in_minutes() ) < minutes_apart) {
      if (match_verbose)
	cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
	cerr << " do not match" << endl;
      return false;
    }
  }

  if (check_coordinates) {

    if (match_verbose)
      cerr << "  Seeking ra=" << entry.position.ra().getHMS()
	   << " have ra=" << have.position.ra().getHMS();

    diff = have.position.ra().getDegrees()-entry.position.ra().getDegrees();
    if (fabs(diff) < RA_deg_apart) {
      if (match_verbose)
	cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }

    if (match_verbose)
      cerr << "  Seeking dec=" << entry.position.dec().getHMS()
	   << " have dec=" << have.position.dec().getHMS();

    diff= have.position.dec().getDegrees()-entry.position.dec().getDegrees();
    if (fabs(diff) < DEC_deg_apart) {
      if (match_verbose)
	cerr << " ... match found" << endl;
    }
    else {
      if (match_verbose)
	cerr << "... no match" << endl;
      return false;
    }
  }

  if (match_verbose)
    cerr << "Pulsar::Database::Criterion::match found" << endl;
  
  return true;
}


// //////////////////////////////////////////////////////////////////////
//
// Pulsar::Database
//
// A group of Pulsar::Database::Entry objects
//

//! Null constructor
Pulsar::Database::Database ()
{
  path = "unset";
}

Pulsar::Database::Database (const char* filename)
{
  path = "unset";

  if (verbose)
    cerr << "Pulsar::Database load " << filename << endl;

  load (filename);
}

string get_current_path ()
{
  unsigned size = 128;
  char* fullpath = new char [size];

  while (getcwd(fullpath, size) == 0) {
    delete fullpath;
    if (errno != ERANGE)
      throw Error (FailedSys, "get_current_path", "getcwd");
    size *= 2;
    fullpath = new char[size];
  }

  string retval = fullpath;
  delete fullpath;
  return retval;
}



/*! This constructor scans the given directory for calibrator files
  ending in the extensions specified in the second argument.
*/      
Pulsar::Database::Database (string _path, const vector<string>& extensions)
{
  string current = get_current_path ();

  if (chdir(_path.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+_path+")");

  path = get_current_path ();

  vector<string> filenames;
  vector<string> patterns (extensions.size());

  for (unsigned i = 0; i < extensions.size(); i++)
    patterns[i] = "*." + extensions[i];

  dirglobtree (&filenames, "", patterns);

  if (chdir(current.c_str()) != 0)
    throw Error (FailedSys, "Pulsar::Database", "chdir("+current+")");


  if (verbose)
    cerr << "Pulsar::Database " << filenames.size() 
         << " calibrator files found" << endl;
  
  Reference::To<Pulsar::Archive> newArch;
  
  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try {

    if (verbose)
      cerr << "Pulsar::Database loading "
	   << filenames[ifile] << endl;
    
    newArch = Archive::load(filenames[ifile]);
    
    if (verbose)
      cerr << "Pulsar::Database create new Entry" << endl;
    
    Entry new_entry(*newArch);
    entries.push_back(new_entry);
    
  }
  catch (Error& error) {
    cerr << "Pulsar::Database error" << error.get_message() << endl;
  }

  if (verbose)
    cerr << "Pulsar::Database constructed with "
         << entries.size() << " Entries" << endl; 
  
}

//! Destructor
Pulsar::Database::~Database ()
{
}

//! Loads an entire database from a file
void Pulsar::Database::load (const char* dbase_filename)
{
  FILE* fptr = fopen (dbase_filename, "r");
  if (!fptr)
    throw Error (FailedCall, "Pulsar::Database::load fopen");

  bool old_style = false;

  char temp[4096];
  int scanned = fscanf (fptr, "Pulsar::Database::path %s\n", temp);
  if (!scanned) {
    rewind (fptr);
    scanned = fscanf (fptr, "Pulsar::Calibration::Database::path %s\n", temp);
    if (scanned)
      cerr << "Pulsar::Database::load old database summmary file" << endl;
    else
      throw Error (InvalidParam, "Pulsar::Database::load",
                   "%s is not a database file", dbase_filename);
    old_style = true;
  }

  path = temp;

  if (verbose)
    cerr << "Pulsar::Database::load setting path = "
	 << path << endl;

  int useful = 0;

  if (!old_style)
    scanned = fscanf (fptr, "Pulsar::Database # of entries = %d\n", &useful);
  else
    scanned = fscanf (fptr, "Pulsar::Calibration::Database # of entries = %d\n", &useful);

  if (verbose)
    cerr << "Pulsar::Database::load resizing for "
	 << useful << " entries" << endl;

  entries.resize(useful);

  for (unsigned ie=0; ie<entries.size(); ie++) {
    fgets (temp, 4096, fptr);

    if (verbose)
      cerr << "Pulsar::Database::load '"<< temp << "'" << endl;

    entries[ie].load(temp);
  }
  fclose (fptr);
}

//! Unloads entire database to file
void Pulsar::Database::unload (const char* dbase_filename)
{
  string filename = path + "/";
  filename += dbase_filename;

  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::Database::unload" 
		 "fopen (" + filename + ")");
  
  fprintf (fptr, "Pulsar::Database::path %s\n", path.c_str());
  fprintf (fptr, "Pulsar::Database # of entries = %d\n", entries.size());

  string out;
  for (unsigned ie=0; ie<entries.size(); ie++) {
    entries[ie].unload(out);
    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);

}


//! returns a vector of Entry instances that match the criterion
vector<Pulsar::Database::Entry> 
Pulsar::Database::all_matching (const Criterion& criterion) const
{
  if (verbose)
    cerr << "Pulsar::Database::all_matching " << entries.size()
         << " entries" << endl;
  
  vector<Pulsar::Database::Entry> matches;
  
  for (unsigned j = 0; j < entries.size(); j++)   
    if (criterion.match (entries[j]))
      matches.push_back(entries[j]);
  
  return matches;
}

static Pulsar::Database::Criterion default_criterion;

//! Get the default matching criterion for PolnCal observations
Pulsar::Database::Criterion
Pulsar::Database::get_default_criterion ()
{
  return default_criterion;
}

void Pulsar::Database::set_default_criterion (const Criterion& criterion)
{
  default_criterion = criterion;
}




//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Entry 
Pulsar::Database::PolnCal_match (Pulsar::Archive* arch,
				 Calibrator::Type calType,
				 bool only_observations)
{
  Criterion criterion = get_default_criterion();

  criterion.entry = Entry (*arch);
  criterion.entry.obsType = Signal::PolnCal;

  vector<Pulsar::Database::Entry> matches = all_matching (criterion);

  if (!only_observations) {

    criterion.entry.obsType = Signal::Calibrator;
    criterion.entry.calType = calType;

    vector<Pulsar::Database::Entry> matches2 = all_matching (criterion);

    unsigned n = matches.size();
    matches.resize( matches.size() + matches2.size() );
    for (unsigned i=0; i<matches2.size(); i++)
      matches[n+i] = matches2[i];

  }

  if (matches.size() == 0)
    throw Error (InvalidParam,
		 "Pulsar::Database::PolnCal_match",
		 "no %s Calibrator found", Calibrator::Type2str (calType));

  return closest_match (criterion, matches);
}

//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Entry 
Pulsar::Database::allsky_match (Pulsar::Archive* arch,
				Calibrator::Type calType,
				double minutes_apart)
{
  Criterion criterion = get_default_criterion();
  criterion.check_coordinates = false;
  criterion.minutes_apart = minutes_apart;

  criterion.entry = Entry (*arch);

  criterion.entry.obsType = Signal::Calibrator;
  criterion.entry.calType = calType;

  vector<Pulsar::Database::Entry> matches;
  matches = all_matching (criterion);

  if (matches.size() == 0)
    throw Error (InvalidParam,
		 "Pulsar::Database::Reception_match",
		 "no %s Calibrator found", Calibrator::Type2str (calType));

  return closest_match (criterion, matches);
}



//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Entry 
Pulsar::Database::closest_match (const Criterion& criterion,
				 const vector<Entry>& entries)
{
  if (entries.size() == 0)
    throw Error (InvalidParam, "Pulsar::Database::closest_match",
		 "empty Entry vector");

  if (verbose)
    cerr << "Database::closest_match selecting from "
	 << entries.size() << " candidates" << endl;
  
  MJD midtime = criterion.entry.time;
  double best_diff = fabs( (entries[0].time - midtime).in_minutes() );
  unsigned the_one = 0;
  
  for (unsigned i = 0; i < entries.size(); i++) {

    double diff = fabs( (entries[i].time - midtime).in_minutes() );

    if (diff < best_diff) {
      the_one = i;
      best_diff = diff;
    }

  }
  
  return entries[the_one];
}


/*! This routine is given a pointer to a Pulsar::Archive. It scans the
  database for all FluxCal observations that were made within one month
  of the observation mid-time and uses these to construct a
  Pulsar::FluxCalibrator class which can then be used to perform a
  flux calibration of the original pulsar observation.
*/      
Pulsar::FluxCalibrator* 
Pulsar::Database::generateFluxCalibrator (Archive* arch, bool allow_raw) try {

  Entry match = allsky_match (arch, Calibrator::Flux, long_time_scale);
  Reference::To<Archive> archive = Archive::load( get_filename(match) );
  return new FluxCalibrator (archive);

}
catch (Error& error) {
  
  if (verbose)
    cerr << "Pulsar::Database::generateFluxCalibrator failure"
      " generating processed FluxCal\n" << error.get_message() << endl;
  
  if (allow_raw)
    return rawFluxCalibrator (arch);
  
  else
    throw error += "Pulsar::Database::generateFluxCalibrator";
  
}

Pulsar::FluxCalibrator* 
Pulsar::Database::rawFluxCalibrator (Pulsar::Archive* arch)
{
  Criterion criterion = get_default_criterion();
  criterion.check_coordinates = false;
  criterion.minutes_apart = long_time_scale;

  criterion.entry = Entry (*arch);
  criterion.entry.obsType = Signal::FluxCalOn;
  vector<Pulsar::Database::Entry> oncals = all_matching (criterion);

  if (!oncals.size())
    throw Error (InvalidState, 
                 "Pulsar::Database::generateFluxCalibrator",
                 "no FluxCalOn observations found to match observation");

  criterion.entry.obsType = Signal::FluxCalOff;
  vector<Pulsar::Database::Entry> offcals = all_matching (criterion);

  if (!offcals.size())
    throw Error (InvalidState,
                 "Pulsar::Database::generateFluxCalibrator",
                 "no FluxCalOff observations found to match observation");


  Reference::To<FluxCalibrator> fluxcal = new FluxCalibrator;
  Reference::To<Archive> archive;

  unsigned ifile = 0;

  for (ifile=0; ifile < oncals.size(); ifile++)
    fluxcal->add_observation (Archive::load( get_filename(oncals[ifile]) ));

  for (ifile=0; ifile < oncals.size(); ifile++)
    fluxcal->add_observation (Archive::load( get_filename(offcals[ifile]) ));

  return fluxcal.release();
}
  

/*! This routine is given a pointer to a Pulsar::Archive. It scans the
  database for all PolnCal observations from the same part of sky that
  were made within 8 hours of the observation mid-time and uses these
  to construct a Pulsar::PolnCalibrator class which can then be used
  to perform a polarisation calibration of the original pulsar
  observation. */

Pulsar::PolnCalibrator* 
Pulsar::Database::generatePolnCalibrator (Archive* arch, Calibrator::Type m)
{
  if (!arch)
    throw Error (InvalidParam, "Database::generatePolnCalibrator",
		 "no Pulsar::Archive given");
  
  if (verbose)
    cerr << "Pulsar::Database::generatePolnCalibrator search for " 
         << Calibrator::Type2str (m) << " match" << endl;

  // the Calibrator must contain an observation of the Reference source
  bool only_observations = m == Pulsar::Calibrator::Hybrid;

  Database::Entry entry = PolnCal_match (arch, m, only_observations);
  
  if (verbose)
    cout << "Constructing PolnCalibrator from file " << entry.filename << endl;
  
  Reference::To<Pulsar::Archive> polcalarch;
  polcalarch = Pulsar::Archive::load(get_filename(entry));

  if (entry.obsType == Signal::Calibrator)
    // if a solved model, return the solution
    return new Pulsar::PolnCalibrator (polcalarch);

  // otherwise, construct a solution
  Reference::To<Pulsar::ReferenceCalibrator> artificial_cal;

  switch (m) {
    
  case Pulsar::Calibrator::Hybrid:
  case Pulsar::Calibrator::SingleAxis:
    artificial_cal = new Pulsar::SingleAxisCalibrator (polcalarch);
    break;
    
  case Pulsar::Calibrator::Polar:
    artificial_cal = new Pulsar::PolarCalibrator (polcalarch);
    break;
    
  default:
    cerr << "Pulsar::Database::generatePolnCalibrator"
      " unknown type" << endl;
    break;
    
  }
  
  if (m == Pulsar::Calibrator::Hybrid)
    return generateHybridCalibrator (artificial_cal, arch);

  return artificial_cal.release();
}

Pulsar::HybridCalibrator* 
Pulsar::Database::generateHybridCalibrator 
(ReferenceCalibrator* arcal, Archive* arch)
{
  if (!arch) throw Error (InvalidParam,
			  "Database::generateHybridCalibrator",
			  "no Pulsar::Archive given");
  if (!arcal) throw Error (InvalidParam,
			    "Database::generateHybridCalibrator",
			    "no Pulsar::ReferenceCalibrator given");
 
  if (verbose)
    cerr << "Pulsar::Database::generateHybridCalibrator" << endl;

  Entry entry;

  try {

    if (verbose)
      cerr << "  Attempting to find a matching Britton Model" << endl;
    entry = allsky_match (arch, Calibrator::Britton, long_time_scale);

  }
  catch (Error& error) {

    try {

      if (verbose)
	cerr << "  Attempting to find a matching Hamaker Model" << endl;
      
      entry = allsky_match (arch, Calibrator::Hamaker, long_time_scale);

    }
    catch (Error& error) {
      if (verbose)
	cerr << "  No full polarimetric models found" << endl;
      return 0;
    }
    
  }

  Reference::To<Pulsar::HybridCalibrator> hybrid;

  try {

    Reference::To<Pulsar::Archive> polncalarch;
    polncalarch = Pulsar::Archive::load(get_filename(entry));

    Reference::To<Pulsar::PolnCalibrator> polncal;
    polncal = new Pulsar::PolnCalibrator (polncalarch);

    Reference::To<Pulsar::CalibratorStokes> calstokes;
    calstokes = polncalarch->get<CalibratorStokes>();

    hybrid = new Pulsar::HybridCalibrator (polncal);

    hybrid->set_reference_input (calstokes);
    hybrid->set_reference_observation (arcal);

  }
  catch (Error& error) {
    throw error += "Pulsar::Database::generateHybridCalibrator";
  }

  return hybrid.release();

}

//! Returns the full pathname of the Entry filename
string Pulsar::Database::get_filename (const Entry& entry) const
{
  return path + "/" + entry.filename;
}

//! Returns the full pathname of the Entry filename
string Pulsar::Database::get_path () const
{
  return path;
}

