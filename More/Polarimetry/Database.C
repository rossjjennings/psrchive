#include "Pulsar/Database.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/FluxCalibrator.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/CalibratorStokes.h"

#include "Pulsar/Archive.h"

#include "Error.h"

#include "Stokes.h"
#include "Jones.h"
#include "MJD.h"
#include "sky_coord.h"

#include "dirutil.h"
#include <unistd.h> 


bool Pulsar::Database::verbose = false;


// //////////////////////////////////////////////////////////////////////
//
// Pulsar::Database::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

static char buffer[512];
static char mjdstr[64];
static char instr[32];
static char posnstr1[16];
static char posnstr2[16];
static char typestr[32];
static char fnstr[256];

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
  
  instrument = "unset";
  filename = "unset";     // relative path of file
}

//! Construct from a Pulsar::Archive
Pulsar::Database::Entry::Entry (const Pulsar::Archive& arch)
{
  obsType = arch.get_type();

  const Pulsar::Backend* backend = arch.get<Backend>();
  if (!backend)
    throw Error (InvalidState, "Pulsar::Database::Entry",
		 "Archive has no Backend Extension");

  if (obsType == Signal::Calibrator) {

    const PolnCalibratorExtension* pcext = arch.get<PolnCalibratorExtension>();

    if (!pcext)
      throw Error (InvalidState, "Pulsar::Database::Entry",
		   "Archive::get_type==Signal::Calibrator but"
		   " no PolnCalibratorExtension");

    calType = pcext->get_type();

    time = pcext->get_epoch();

  }
  else
    time = ( arch.start_time() + arch.end_time() ) / 2.0;

  position = arch.get_coordinates();
  bandwidth = arch.get_bandwidth();
  frequency = arch.get_centre_frequency();
  nchan = arch.get_nchan();
  filename = arch.get_filename();

  instrument = backend->get_name();

}

//! Destructor
Pulsar::Database::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::Database::Entry::load (const char* str) 
{
  int s = sscanf (str, "%s %s %s %s %s %lf %lf %d %s", fnstr, typestr, 
		  posnstr1, posnstr2, mjdstr, 
                  &bandwidth, &frequency, &nchan, instr);
  if (s != 9)
    throw Error (FailedSys, "Pulsar::Database::Entry::load",
                 "sscanf(%s) != 9", str);

  time.Construct(mjdstr);

  string useful = posnstr1;
  useful += " ";
  useful += posnstr2;

  sky_coord temp(useful.c_str());
  position = temp;
  
  if (strcmp(typestr,"Pulsar") == 0)
    obsType = Signal::Pulsar;
  else if (strcmp(typestr,"PolnCal") == 0)
    obsType = Signal::PolnCal;
  else if (strcmp(typestr,"FluxCalOn") == 0)
    obsType = Signal::FluxCalOn;
  else if (strcmp(typestr,"FluxCalOff") == 0)
    obsType = Signal::FluxCalOff;
  else {
    obsType = Signal::Calibrator;
    calType = Calibrator::str2Type (typestr);
  }

  instrument = instr;
  filename = fnstr;
  
}

// unload to a string
void Pulsar::Database::Entry::unload (string& str)
{
  string retval;
  
  retval += filename + " ";
  
  switch (obsType) {
  case Signal::Unknown:
    retval += "Unknown ";
    break;
  case Signal::Pulsar:
    retval += "Pulsar ";
    break;
  case Signal::PolnCal:
    retval += "PolnCal ";
    break;
  case Signal::FluxCalOn:
    retval += "FluxCalOn ";
    break;
  case Signal::FluxCalOff:
    retval += "FluxCalOff ";
    break;
  case Signal::Calibrator:
    {
      retval += Calibrator::Type2str (calType);
      retval += " ";
    }
    break;
  }

  retval += position.getRaDec().getHMSDMS();
  retval += " ";

  strcpy (mjdstr, time.printdays(15).c_str());
  retval += mjdstr;
  retval += " ";
  
  sprintf (buffer, "%lf %lf %d %s", bandwidth, frequency,
	   nchan, instrument.c_str());
  
  retval += buffer;
  
  str = retval;
}


Pulsar::Database::Criterion::Criterion ()
{
  minutes_apart = 0.0;
  RA_deg_apart  = 0.0;
  DEC_deg_apart = 0.0;

  check_instrument  = true;
  check_frequency   = true;
  check_bandwidth   = true;
  check_obs_type    = true;
  check_time        = true;
  check_coordinates = true;
}


//! returns true if this matches observation parameters
bool Pulsar::Database::Criterion::match (const Entry& _entry) const
{

  if (verbose)
    cerr << "Pulsar::Database::Criterion::match" << endl;
 
  if (check_obs_type) {

    if (entry.obsType == _entry.obsType) {
      if (verbose)
	cerr << "  Observation type match found" << endl; 
    }
    else {
      if (verbose)
	cerr << "  Observation types do not match" << endl;
      return false;
    }

    if (entry.obsType == Signal::Calibrator) {

      if (entry.calType == _entry.calType) {
	if (verbose)
	  cerr << "  Calibrator type match found" << endl; 
      }
      else {
	if (verbose)
	  cerr << "  Calibrator types do not match" << endl;
	return false;
      }

    }

  }

  if (check_bandwidth) {

    if (entry.bandwidth==_entry.bandwidth) {
      if (verbose)
	cerr << "  Bandwidth match found" << endl; 
    }
    else {
      if (verbose)
	cerr << "  Bandwidth does not match" << endl;
      return false;
    }
  }

  if (check_frequency) {

    if (entry.frequency==_entry.frequency) {
      if (verbose)
	cerr << "  Frequency match found" << endl; 
    }
    else {
      if (verbose)
	cerr << "  Frequency does not match" << endl;
      return false;
    }
  }

  if (check_instrument) {

    if (entry.instrument==_entry.instrument) {
      if (verbose)
	cerr << "  Instrument match found" << endl;
    }
    else {
      if (verbose)
	cerr << "  Instrument does not match" << endl;
      return false;
    }
  }
  
  if (verbose) {
    cerr <<
      "  MidTime that: " << _entry.time.in_minutes() << " min" << "\n"
      "  MidTime this: " << entry.time.in_minutes() << " min" << "\n"
      "  Time difference: " << fabs( (_entry.time-entry.time).in_minutes() )
         << " minutes" << endl <<
      "  Threshold difference:" << minutes_apart << " min" << endl;
  }

  if (check_time) {
    if (fabs( (_entry.time - entry.time).in_minutes() ) < minutes_apart) {
      if (verbose)
	cerr << "  Time match found" << endl;
    }
    else {
      if (verbose)
	cerr << "  Times do not match" << endl;
      return false;
    }
  }

  if (verbose) {
    cerr << "  RA that = " << _entry.position.ra().getHMS() << endl;
    cerr << "  RA this = " << entry.position.ra().getHMS() << endl;  
  }

  double diff;

  if (check_coordinates) {

    diff = _entry.position.ra().getDegrees()-entry.position.ra().getDegrees();
    if (fabs(diff) < RA_deg_apart) {
      if (verbose)
	cerr << "  RA match found" << endl;
    }
    else {
      if (verbose)
	cerr << "  RA does not match" << endl;
      return false;
    }
  }

  if (verbose) {
    cerr << "  DEC that = " << _entry.position.dec().getDMS() << endl;
    cerr << "  DEC this = " << entry.position.dec().getDMS() << endl;
  }

  if (check_coordinates) {

    diff= _entry.position.dec().getDegrees()-entry.position.dec().getDegrees();
    if (fabs(diff) < DEC_deg_apart) {
      if (verbose)
	cerr << "  DEC match found" << endl;
    }
    else {
      if (verbose)
	cerr << "  DEC does not match" << endl;
      return false;
    }
  }

  if (verbose)
    cerr << "  This is a match." << endl;
  
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


/*! This constructor scans the given directory for calibrator files
  ending in the extensions specified in the second argument.
*/      
Pulsar::Database::Database (string _path,
			    const vector<string>& extensions)
{
  char original[4096];
  getcwd(original, 4096);
  
  path = _path;

  if (path.at(path.length()-1) != '/')
    path += "/";

  if (chdir(path.c_str()) != 0)
    throw Error (FailedCall, "Pulsar::Database chdir");

  char useful[4096];
  getcwd(useful, 4096);

  if (verbose)
    cout << "Pulsar::Database setting path to " << useful << endl;

  path = useful;

  vector<string> filenames;

  for (unsigned i = 0; i < extensions.size(); i++) {
    string glob = "*." + extensions[i];
    dirglob (&filenames, glob);
  } 

  if (verbose)
    cerr << "Pulsar::Database " << filenames.size() 
         << " calibrator files found" << endl;
  
  Reference::To<Pulsar::Archive> newArch;
  
  for (unsigned ifile=0; ifile<filenames.size(); ifile++) {
    try {

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
      cerr << error << endl;
    }
  }

  if (verbose)
    cerr << "Pulsar::Database constructed with "
         << entries.size() << " Entries" << endl; 
  
  chdir(original);
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

//! Get the default matching criterion for PolnCal observations
Pulsar::Database::Criterion
Pulsar::Database::get_default_PolnCal_criterion ()
{
  Criterion criterion = get_default_criterion ();
  criterion.minutes_apart = 720.0;
  criterion.RA_deg_apart  = 5.0;
  criterion.DEC_deg_apart = 5.0;
  return criterion;
}

//! Get the default matching criterion for FluxCal observations
Pulsar::Database::Criterion
Pulsar::Database::get_default_FluxCal_criterion ()
{
  Criterion criterion = get_default_criterion ();
  criterion.minutes_apart = 43200.0;
  criterion.check_coordinates = false;
  return criterion;
}

//! Get the default matching criterion for Reception model solutions
Pulsar::Database::Criterion
Pulsar::Database::get_default_Reception_criterion ()
{
  Criterion criterion = get_default_criterion ();
  criterion.minutes_apart = 43200.0;
  criterion.check_coordinates = false;
  return criterion;
}


//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Entry 
Pulsar::Database::PolnCal_match (Pulsar::Archive* arch,
				 Calibrator::Type calType,
				 bool only_observations)
{
  Criterion criterion = get_default_PolnCal_criterion();

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
		 "Pulsar::Database::PolnCalibrator_match",
		 "no %s Calibrator found", Calibrator::Type2str (calType));

  return closest_match (criterion, matches);
}

//! Returns one Entry that matches the given parameters and is nearest in time.
Pulsar::Database::Entry 
Pulsar::Database::Reception_match (Pulsar::Archive* arch,
				   Calibrator::Type calType)
{
  Criterion criterion = get_default_Reception_criterion();

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
  database for all FluxCal observations that were made within 24 hours
  of the observation mid-time and uses these to construct a
  Pulsar::FluxCalibrator class which can then be used to perform a
  flux calibration of the original pulsar observation.
*/      
Pulsar::FluxCalibrator* 
Pulsar::Database::generateFluxCalibrator (Pulsar::Archive* arch)
{
  Criterion criterion = get_default_criterion();
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

  vector<Pulsar::Archive*> fluxcalarchs;
  
  for (unsigned i = 0; i < oncals.size(); i++)
    fluxcalarchs.push_back(Pulsar::Archive::load(get_filename(oncals[i])));
			   
  for (unsigned i = 0; i < offcals.size(); i++)
    fluxcalarchs.push_back(Pulsar::Archive::load(get_filename(offcals[i])));
  
  if (verbose) {
    cout << "Constructing FluxCalibrator from these files:" << endl;
    for (unsigned i = 0; i < oncals.size(); i++)
      cout << get_filename(oncals[i]) << endl;
    for (unsigned i = 0; i < offcals.size(); i++)
      cout << get_filename(offcals[i]) << endl;
  }
  
  Pulsar::FluxCalibrator* retval = new Pulsar::FluxCalibrator(fluxcalarchs);
  
  return retval;
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
    entry = Reception_match (arch, Calibrator::Britton);

  }
  catch (Error& error) {

    try {

      if (verbose)
	cerr << "  Attempting to find a matching Hamaker Model" << endl;
      
      entry = Reception_match (arch, Calibrator::Hamaker);

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

