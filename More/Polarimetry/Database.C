/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Database.h"
#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/HybridCalibrator.h"
#include "Pulsar/FluxCalibrator.h"

#include "Pulsar/CalibrationInterpolatorExtension.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/FeedExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/ChannelSubsetMatch.h"

#include "ModifyRestore.h"
#include "Error.h"

#include "Stokes.h"
#include "Jones.h"
#include "MJD.h"
#include "sky_coord.h"

#include "dirutil.h"
#include "strutil.h"
#include "pairutil.h"

#include <unistd.h> 
#include <errno.h>
#include <assert.h>

using namespace std;
using namespace Pulsar;

bool Database::cache_last_cal = true;
bool Database::match_verbose = false;


/*! By default, the long time scale is set to four weeks. */
Pulsar::Option<double> 
Database::long_time_scale
(
 "Database::long_time_scale", 60.0 * 24 * 28,

 "Maximum time to flux calibrator [minutes]",

 "The maximum amount of time allowed between the epoch of an observation \n"
 "and the epoch of the flux calibrator and/or reception calibrator used to \n"
 "calibrate it.  By default, the long time scale is set to four weeks."
);


/*! By default, the short time scale is set to two hours. */
Pulsar::Option<double> 
Database::short_time_scale
(
 "Database::short_time_scale", 120.0,

 "Maximum time to poln calibrator [minutes]",

 "The maximum amount of time allowed between the epoch of an observation \n"
 "and the epoch of the reference calibrator used to calibrate it. \n"
 "By default, the long time scale is set to two hours."
);


/*! By default, the maximum angular separation is 5 degrees */
Pulsar::Option<double> 
Database::max_angular_separation 
(
 "Database::max_angular_separation", 5.0,

 "Maximum distance to poln calibrator [degrees]",

 "The maximum separation between the sky coordinates of an observation \n"
 "and that of the reference calibrator used to calibrate it."
);


/*! By default, the maximum centre frequency difference is 1 Hz */
Pulsar::Option<double> 
Database::max_centre_frequency_difference
(
 "Database::max_centre_frequency_difference", 1.0,

 "Maximum difference in centre frequency of calibrator [Hz]",

 "The maximum difference between the centre frequency of an observation \n"
 "and that of the reference calibrator used to calibrate it."
);


/*! By default, the maximum bandwidth difference is 1 Hz */
Pulsar::Option<double> 
Database::max_bandwidth_difference
(
 "Database::max_bandwidth_difference", 1.0,

 "Maximum difference in bandwidth of calibrator [Hz]",

 "The maximum difference between the bandwidth of an observation \n"
 "and that of the reference calibrator used to calibrate it."
);


/*! This null parameter is intended only to improve code readability */
const Pulsar::Archive* Database::any = 0;

// //////////////////////////////////////////////////////////////////////
//
// Database::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Database::Entry::init ()
{
  obsType = Signal::Unknown;

  bandwidth = 0.0;
  frequency = 0.0; 
  
  receiver = "unset";
  instrument = "unset";
  filename = "unset";
}

void Database::StaticEntry::init ()
{
  Entry::init ();
  nchan = 0; 
}

//! Construct from a Pulsar::Archive
Database::Entry::Entry (const Pulsar::Archive* arch)
{
  init ();

  if (!arch)
    return;
  
  obsType = arch->get_type();

  const Pulsar::Backend* backend = arch->get<Backend>();
  if (!backend)
    throw Error (InvalidParam, "Database::Entry",
		 "Archive has no Backend Extension");

  const Pulsar::Receiver* receiver_ext = arch->get<Receiver>();
  if (!receiver_ext)
    throw Error (InvalidParam, "Database::Entry",
		 "Archive has no Receiver Extension");

  calType = 0;
  
  if (obsType == Signal::Calibrator)
  {
    {
      auto ext = arch->get<CalibratorExtension>();
      if (ext)
	calType = ext->get_type();
    }

    {
      auto ext = arch->get<CalibrationInterpolatorExtension>();
      if (ext)
	calType = ext->get_type();
    }
    
    if (!calType)
      throw Error (InvalidState, "Database::Entry",
		   "Archive::get_type==Signal::Calibrator but"
		   " neither CalibratorExtension"
		   " nor CalibrationInterpolatorExtension");
  }

  position = arch->get_coordinates();
  bandwidth = arch->get_bandwidth();
  frequency = arch->get_centre_frequency();
  filename = arch->get_filename();

  instrument = backend->get_name();
  receiver = receiver_ext->get_name();
}

//! Construct from a Pulsar::Archive
Database::StaticEntry::StaticEntry (const Pulsar::Archive* arch)
  : Entry (arch)
{
  if (!arch)
    return;
  
  nchan = arch->get_nchan();

  if (obsType == Signal::Calibrator)
  {
    const CalibratorExtension* ext = arch->get<CalibratorExtension>();
    time = ext->get_epoch();
  }
  else
  {
    unsigned nsubint = arch->get_nsubint();

    if (nsubint == 0)
      throw Error (InvalidParam, "Database::Entry",
		   "Archive has no Integrations");

    // assign the midpoint epoch to the observation
    MJD epoch1 = arch->get_Integration(0)->get_epoch();
    MJD epochN = arch->get_Integration(nsubint-1)->get_epoch();

    time = (epoch1 + epochN) / 2;
  }
}

//! Construct from a Pulsar::Archive
Database::InterpolatorEntry::InterpolatorEntry (const Pulsar::Archive* arch)
  : Entry (arch)
{
  if (!arch)
    return;

  auto interpolator = arch->get<const CalibrationInterpolatorExtension>();
  if (!interpolator)
    throw Error (InvalidParam, "Database::InterpolatorEntry ctor",
		 arch->get_filename() +
		 " does not contain a CalibrationInterpolatorExtension");

  start_time = interpolator->get_minimum_epoch();
  end_time = interpolator->get_maximum_epoch();
}

//! Destructor
Database::Entry::~Entry ()
{
  
}


Database::Entry* Database::Entry::create (const Archive* archive)
{
  if (archive->get<const CalibrationInterpolatorExtension>())
    return new InterpolatorEntry (archive);
  else
    return new StaticEntry (archive);
}


// load from ascii string
Database::Entry* Database::Entry::load (const string& str) try
{
  const char* whitespace = " \t\n";
  string line = str;

  // /////////////////////////////////////////////////////////////////
  // filename
  string filename = stringtok (line, whitespace);
  
  // /////////////////////////////////////////////////////////////////
  // type
  string typestr = stringtok (line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // RA DEC
  string coordstr = stringtok (line, whitespace);
  coordstr += " " + stringtok (line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // MJD
  string mjdstr  = stringtok (line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // bandwidth
  string bwstr  = stringtok (line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // frequency
  string freqstr  = stringtok (line, whitespace);

  Reference::To<Entry> entry;
  
  if (mjdstr.find ("-") != string::npos)
  {
    // cerr << "Database::Entry::load new InterpolatorEntry mjd range=" << mjdstr << endl;
   
    string mjd1 = stringtok (mjdstr, "-");
     
    // the MJD string contains a range of MJDs; therefore
    auto interpolator_entry = new InterpolatorEntry;
    interpolator_entry->start_time = MJD (mjd1);
    interpolator_entry->end_time = MJD (mjdstr);

    entry = interpolator_entry;
  }
  else
  {
    // cerr << "Database::Entry::load new StaticEntry mjd=" << mjdstr << endl;
    
    auto static_entry = new StaticEntry;
    static_entry->time = MJD (mjdstr);

    // /////////////////////////////////////////////////////////////////
    // nchan
    string nchan = stringtok (line, whitespace);
    static_entry->nchan = fromstring<unsigned> (nchan);
    entry = static_entry;
  }

  entry->filename = filename;
    
  try
  {
    entry->obsType = Signal::string2Source(typestr);
  }
  catch (Error& e)
  {
    entry->obsType = Signal::Calibrator;

    // cerr << "Database::Entry::load name=" << typestr << endl;
    entry->calType = Calibrator::Type::factory (typestr);
    // cerr << "Database::Entry::load type=" << calType->get_name() << endl;
  }
    
  entry->position = sky_coord (coordstr.c_str());
  entry->bandwidth = fromstring<double> (bwstr);
  entry->frequency = fromstring<double> (freqstr);

  // /////////////////////////////////////////////////////////////////
  // instrument
  entry->instrument = stringtok (line, whitespace);

  // /////////////////////////////////////////////////////////////////
  // receiver
  entry->receiver = stringtok (line, whitespace);

  if (entry->receiver.length() == 0)
    throw Error (InvalidParam, "Database::Entry::load",
                 "Could not parse '"+str+"'");

  return entry.release();
}
catch (Error& error)
{
  throw error += "Database::Entry::load";
}

// unload to a string
void Database::Entry::unload (string& retval)
{
  retval = filename + " ";
  
  if (obsType == Signal::Calibrator)
    retval += calType->get_name();
  else
    retval += Signal::Source2string (obsType);

  retval += " ";
  retval += position.getRaDec().getHMSDMS();

  retval += " ";
  retval += get_time_str ();

  retval += " ";
  retval += tostring(bandwidth);

  retval += " ";
  retval += tostring(frequency);

  retval += " ";
  retval += get_nchan_str ();

  retval += " " + instrument + " " + receiver;
}

string Database::StaticEntry::get_time_str () const
{
  return time.printdays(15);
}

string Database::InterpolatorEntry::get_time_str () const
{
  return start_time.printdays(4) + "-" + end_time.printdays(4);
}

string Database::StaticEntry::get_nchan_str () const
{
  return tostring(nchan);
}

string Database::InterpolatorEntry::get_nchan_str () const
{
  return "";
}

//! Returns the full pathname of the Entry filename
string Database::Entry::get_filename () const
{
  return filename;
}

namespace Pulsar
{
  bool same (const Reference::To<const Calibrator::Type>& a, 
	     const Reference::To<const Calibrator::Type>& b)
  {
    if (!a && !b)
      return true;

    if (!a || !b)
      return false;

    return a->is_a(b) || b->is_a(a);
  }
}


bool Database::Entry::equals (const Entry* that) const
{
  // cerr << "Database::Entry::equals this=" << (void*) this << endl;
    
  return this->obsType == that->obsType &&
    same( this->calType, that->calType ) &&
    this->bandwidth == that->bandwidth &&
    this->frequency == that->frequency &&
    this->instrument == that->instrument &&
    this->receiver == that->receiver &&
    this->position.angularSeparation(that->position).getDegrees() < 0.1;
}

bool Database::StaticEntry::equals (const Entry* that) const
{
  // cerr << "Database::StaticEntry::equals this=" << (void*) this << endl;

  if (!Entry::equals (that))
    return false;
  
  auto like = dynamic_cast<const StaticEntry*> (that);
  if (!like)
    return false;

  // cerr << "Database::StaticEntry::equals " << this->time << " " << like->time << endl;
  
  return fabs( (this->time - like->time).in_seconds() ) < 10.0;
}

bool Database::InterpolatorEntry::equals (const Entry* that) const
{
  // cerr << "Database::InterpolatorEntry::equals this=" << (void*) this << endl; 

  if (!Entry::equals (that))
    return false;
  
  auto like = dynamic_cast<const InterpolatorEntry*> (that);
  if (!like)
    return false;

  return
    fabs( (this->start_time - like->start_time).in_seconds() ) < 10.0 &&
    fabs( (this->end_time - like->end_time).in_seconds() ) < 10.0;
}

bool Database::Entry::less_than (const Entry* that) const
{
  if (!that)
    throw Error (InvalidParam, "Database::Entry::less_than",
                 "null pointer passed as argument");

  // cerr << "lt this=" << (void*)this << " that=" << (void*)that << endl;

  if (this->instrument < that->instrument)
    return true;
  else if (this->instrument > that->instrument) 
    return false;

  if (this->receiver < that->receiver)
    return true;
  else if (this->receiver > that->receiver) 
    return false;

  if (this->frequency < that->frequency)
    return true;
  else if (this->frequency > that->frequency) 
    return false;

  if (this->bandwidth < that->bandwidth)
    return true;
  else if (this->bandwidth > that->bandwidth)
    return false;

  return this->get_effective_time() < that->get_effective_time();
}

ostream& Pulsar::operator << (ostream& os, Database::Sequence sequence)
{
  switch (sequence) {
  case Database::Any:
    return os << "any";
  case Database::CalibratorBefore:
    return os << "before";
  case Database::CalibratorAfter:
    return os << "after";
  }
  return os;
}

istream& Pulsar::operator >> (istream& is, Database::Sequence& sequence)
{
  std::streampos pos = is.tellg();
  string unit;
  is >> unit;

  if (casecmp(unit, "any") || casecmp(unit, "none"))
    sequence = Database::Any;

  else if (casecmp(unit, "after"))
    sequence = Database::CalibratorAfter;

  else if (casecmp(unit, "before"))
    sequence = Database::CalibratorBefore;

  else 
  {
    // replace the text and set the fail bit
    is.seekg (pos);
    is.setstate (ios::failbit);
  }

  return is;
}








Database::Criteria::Criteria ()
{
  minutes_apart = short_time_scale;
  deg_apart  = max_angular_separation;

  sequence = Any;

  check_receiver    = true;
  check_instrument  = true;
  check_frequency   = true;
  check_bandwidth   = true;
  check_obs_type    = true;
  check_time        = true;
  check_coordinates = true;
  check_frequency_array = false;

  match_count = 0;
  diff_degrees = diff_minutes = 0;
}

void Database::Criteria::no_data ()
{
  entry = new StaticEntry;
  check_obs_type    = true;
  check_receiver    = false;
  check_instrument  = false;
  check_frequency   = false;
  check_bandwidth   = false;
  check_time        = false;
  check_coordinates = false;
  check_frequency_array = false;
}

bool freq_close (double f1, double f2)
{
  // bandwidth is in MHz; maximum difference is in Hz
  return fabs(f1 - f2)*1e6 < Database::max_centre_frequency_difference;
}

bool bandwidth_close (double f1, double f2)
{
  // bandwidth is in MHz; maximum difference is in Hz
  return fabs(f1 - f2)*1e6 < Database::max_bandwidth_difference;
}

bool Database::Criteria::compare_times (const MJD& want,
					const MJD& have) const
{
  diff_minutes = (have - want).in_minutes();

  switch (sequence)
  {
  case Any:
  default:
    diff_minutes = fabs( diff_minutes );
    break;
  case CalibratorBefore:
    diff_minutes = -diff_minutes;
    break;
  case CalibratorAfter:
    // do nothing
    break;
  }

  match_report += "\n\t" "difference=" + tostring(diff_minutes) + " minutes "
    "(max=" + tostring(minutes_apart) + ") ... ";

  return minutes_apart == 0
      || (diff_minutes < minutes_apart && diff_minutes >= 0);
}

bool Database::Criteria::bracketed_time (const MJD& want,
					 const pair<MJD,MJD>& have) const
{
  match_report += "\n\t" "range="
    + tostring(have.first) + "-" + tostring(have.second) + " ... ";

  return want > have.first && want < have.second;
}


bool Database::Criteria::compare_coordinates (const sky_coord& want,
					      const sky_coord& have) 
  const
{
  diff_degrees = have.angularSeparation(want).getDegrees();

  match_report += "\n\t" "difference=" + tostring(diff_degrees) + " degrees "
    "(max=" + tostring(deg_apart) + ") ... ";

  return deg_apart == 0 || diff_degrees < deg_apart;
}

std::ostream& operator<< (std::ostream& ostr, const Reference::To<const Calibrator::Type>& type)
{
  ostr << type->get_name();
  return ostr;
}

//! returns true if this matches observation parameters
bool Database::Criteria::match (const Entry* have) const try
{
  if (Calibrator::verbose > 1)
    cerr << "Database::Criteria::match" << endl;
 
  match_report = "";
  match_count = 0;
  diff_degrees = diff_minutes = 0.0;

  if (check_obs_type)
  {
    compare( "obsType", entry->obsType, have->obsType );
    
    if (entry->obsType == Signal::Calibrator)
      compare( "calType", entry->calType, have->calType, &Pulsar::same );
  }

  if (check_receiver)
    compare( "receiver", entry->receiver, have->receiver );

  if (check_instrument)
    compare( "instrument", entry->instrument, have->instrument );

  if (check_frequency)
    compare( "frequency", entry->frequency, have->frequency, &freq_close );

  if (check_bandwidth)
    compare( "bandwidth", entry->bandwidth, have->bandwidth, &bandwidth_close );

  auto static_entry = dynamic_cast<const StaticEntry*> (have);
  
  if (static_entry && check_time)
  {
    Functor< bool (MJD, MJD) >
      predicate (this, &Criteria::compare_times);
    compare( "time", entry->time, static_entry->time, predicate );
  }

  auto interpolator = dynamic_cast<const InterpolatorEntry*> (have);
  if (interpolator)
  {
    Functor< bool (MJD, pair<MJD,MJD>) >
      predicate (this, &Criteria::bracketed_time);

    pair<MJD,MJD> range (interpolator->start_time, interpolator->end_time);
    compare( "time", entry->time, range, predicate );
  }
  
  if (check_coordinates)
  {
    Functor< bool (sky_coord, sky_coord) > 
      predicate (this, &Criteria::compare_coordinates);
    compare( "position", entry->position, have->position, predicate );
  }

  if (static_entry && check_frequency_array)
  {
    // For now this assumes we can accurately recreate the original
    // freq arrays from nchan, bw, and freq.

    ChannelSubsetMatch chan_match;

    match_report += "channel subset ... ";
    if (chan_match.match(static_entry, entry))
    {
      match_report += "match";
      match_count ++;
    }
    else 
    {
      match_report += "no match (" + chan_match.get_reason() + ")";
      return false;
    }
  }

  if (Calibrator::verbose > 1 || match_verbose)
    cerr << "Database::Criteria::match found \n\n"
	 << match_report << endl;
  
  return true;
}
catch (bool f)
{
  if (Calibrator::verbose > 1 || match_verbose)
    cerr << "Database::Criteria::match not found \n\n"
	 << match_report << endl;
  return f;
}

// //////////////////////////////////////////////////////////////////////
//
// Database
//
// A group of Database::Entry objects
//

//! Null constructor
Database::Database ()
{
  path = "unset";
}

Database::Database (const string& filename)
{
  path = "unset";

  if (Calibrator::verbose > 2)
    cerr << "Database load " << filename << endl;

  load (filename);
}

string get_current_path ()
{
  unsigned size = 128;
  char* fullpath = new char [size];

  while (getcwd(fullpath, size) == 0)
  {
    delete [] fullpath;
    if (errno != ERANGE)
      throw Error (FailedSys, "get_current_path", "getcwd");
    size *= 2;
    fullpath = new char[size];
  }

  string retval = fullpath;
  delete [] fullpath;
  return retval;
}

//! Construct a database from archives in a metafile
Database::Database (const std::string& _path, 
			    const std::string& metafile)
{
  vector <string> filenames;
  stringfload (&filenames, metafile);

  string current = get_current_path ();

  if (chdir(_path.c_str()) != 0)
    throw Error (FailedSys, "Database", "chdir("+_path+")");

  path = get_current_path ();

  construct (filenames);

  if (chdir(current.c_str()) != 0)
    throw Error (FailedSys, "Database", "chdir("+current+")");
}


/*! This constructor scans the given directory for calibrator files
  ending in the extensions specified in the second argument.
*/      
Database::Database (const string& _path, 
			    const vector<string>& extensions)
{
  string current = get_current_path ();

  if (chdir(_path.c_str()) != 0)
    throw Error (FailedSys, "Database", "chdir("+_path+")");

  path = get_current_path ();

  vector<string> filenames;
  vector<string> patterns (extensions.size());

  for (unsigned i = 0; i < extensions.size(); i++)
    patterns[i] = "*." + extensions[i];

  dirglobtree (&filenames, "", patterns);

  if (Calibrator::verbose > 2)
    cerr << "Database " << filenames.size() 
         << " calibrator files found" << endl;

  construct (filenames);
  
  if (chdir(current.c_str()) != 0)
    throw Error (FailedSys, "Database", "chdir("+current+")");

}

void Database::construct (const vector<string>& filenames)
{
  ModifyRestore<bool> mod (Profile::no_amps, true);

  Reference::To<Pulsar::Archive> newArch;
  
  for (unsigned ifile=0; ifile<filenames.size(); ifile++) try
  {
    if (filenames[ifile] == "filename")
      continue;

    if (Calibrator::verbose > 1)
      cerr << "Database loading "
	   << filenames[ifile] << endl;
    
    newArch = Archive::load(filenames[ifile]);
    
    if (Calibrator::verbose > 1)
      cerr << "Database create new Entry" << endl;
    
    add (newArch);
  }
  catch (Error& error)
  {
    cerr << "Database error " << error.get_message() << endl;
  }

  if (Calibrator::verbose > 2)
    cerr << "Database::construct "
         << entries.size() << " Entries" << endl; 
}

//! Destructor
Database::~Database ()
{
}

//! Loads an entire database from a file
void Database::load (const string& dbase_filename)
{
  string use_filename = expand (dbase_filename);

  FILE* fptr = fopen (use_filename.c_str(), "r");
  if (!fptr)
    throw Error (FailedCall, "Database::load",
		 "fopen (" + use_filename + ")");

  bool old_style = false;

  char temp[4096];
  int scanned = fscanf (fptr, "Pulsar::Database::path %s\n", temp);
  if (!scanned)
  {
    rewind (fptr);
    scanned = fscanf (fptr, "Pulsar::Calibration::Database::path %s\n", temp);
    if (scanned)
      cerr << "Database::load old database summmary file" << endl;
    else
      throw Error (InvalidParam, "Database::load",
                   use_filename + " is not a database file");
    old_style = true;
  }

  path = temp;

  if (Calibrator::verbose > 2)
    cerr << "Database::load setting path = " << path << endl;

  unsigned count = 0;

  string parse = "Pulsar::Database # of entries = %d\n";
  if (old_style)
    parse = "Pulsar::Calibration::Database # of entries = %d\n";

  if ( fscanf (fptr, parse.c_str(), &count) < 1 )
    cerr << "Database::load failed to read number of entries" << endl;

  if (Calibrator::verbose > 2)
    cerr << "Database::loading " << count << " entries" << endl;

  Entry* entry = 0;

  while (fgets (temp, 4096, fptr)) try
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::load '"<< temp << "'" << endl;

    entry = Entry::load (temp);
    expand_filename (entry);
    add (entry);
  }
  catch (Error& error)
  {
    cerr << "Database::load discarding entry:" << error << endl;
  }

  if (Calibrator::verbose > 2)
    cerr << "Database::load " << entries.size() << " entries" <<endl;

  fclose (fptr);
}
void Database::merge (const Database* other)
{
  for (unsigned ie=0; ie < other->entries.size(); ie++)
    add (other->entries[ie]);
}

//! Unloads entire database to file
void Database::unload (const string& filename)
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Database::unload" 
		 "fopen (" + filename + ")");
  
  fprintf (fptr, "Pulsar::Database::path %s\n", path.c_str());
  fprintf (fptr, "Pulsar::Database # of entries = %u\n", 
	   (unsigned)entries.size());

  string out;
  for (unsigned ie=0; ie<entries.size(); ie++)
  {
    Entry* temp = entries[ie];

    shorten_filename (temp);  // WvS this now shortens entries[ie]
    temp->unload(out);

    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);
}

//! Add the given Archive to the database
void Database::add (const Pulsar::Archive* archive)
{
  if (!archive)
    throw Error (InvalidParam, "Database::add Archive",
		 "null Archive*");
  try
  {
    Entry* entry = Entry::create (archive);
    expand_filename (entry);
    add (entry);
  }
  catch (Error& error)
  {
    throw error += "Database::add Archive";
  }
}

void Database::expand_filename (Entry* entry)
{
  if (path.empty())
    return;

  entry->filename = get_filename (entry);
}

void Database::shorten_filename (Entry* entry)
{
  if (path.empty())
    return;

  if (entry->filename.substr(0, path.length()) == path)
    entry->filename.erase (0, path.length()+1);
}

//! Add the given Archive to the database
void Database::add (Entry* entry) try
{
  if (!entry)
    throw Error (InvalidParam, "Database::add", "null Entry");

  for (unsigned ie=0; ie < entries.size(); ie++) 
  {
    if (entries[ie]->filename == entry->filename)
    {
      cerr << "Database::add replacing current entry: \n\t"
           << entry->filename << endl;
      entries[ie] = entry;
      return;
    }
    else if (entries[ie]->equals (entry))
    {
      cerr << "Database::add keeping newest of duplicate entries:\n\t"
           << entries[ie]->filename << " and\n\t" << entry->filename << endl;
      if ( file_mod_time (get_filename(entry).c_str()) >
	   file_mod_time (get_filename(entries[ie]).c_str()) )
	entries[ie] = entry;
      return;
    }
  }

  entries.push_back (entry);
}
catch (Error& error)
{
  throw error += "Database::add Entry";
}

void Database::all_matching (const Criteria& criteria,
			     vector<const Entry*>& matches) const
{
  if (Calibrator::verbose > 2)
    cerr << "Database::all_matching " << entries.size() << " entries" << endl;

  closest_match = Criteria();

  vector<bool> does_match (entries.size(), false);
  unsigned total_matches = 0;

  for (unsigned j = 0; j < entries.size(); j++)
  {
    if (criteria.match (entries[j]))
    {
      does_match[j] = true;
      total_matches++;
    }
    else
    {
      does_match[j] = false;
      closest_match = Criteria::closest (closest_match, criteria);
    }
  }

  unsigned imatch = matches.size();
  matches.resize (imatch + total_matches);
  for (unsigned j = 0; j < entries.size(); j++)
  {
    if (does_match[j])
    {
      matches[imatch] = entries[j];
      imatch ++;
    }
  }

  assert (imatch == matches.size());
}

const Database::Entry*
Database::best_match (const Criteria& criteria) const
{
  if (Calibrator::verbose > 1)
    cerr << "Database::best_match " << entries.size() << " entries" << endl;
  
  const Entry* best_match = 0;

  closest_match = Criteria();
  
  for (unsigned ient = 0; ient < entries.size(); ient++)
    if (criteria.match (entries[ient]))
      best_match = criteria.best (entries[ient], best_match);
    else
      closest_match = Criteria::closest (closest_match, criteria);

  if (!best_match || best_match->obsType == Signal::Unknown)
    throw Error (InvalidParam, "Pulsar::Calibration::Database::best_match",
                 "no match found");

  return best_match;
}

std::string Database::get_closest_match_report () const
{ 
  if (!closest_match.match_count)
    return "\t" "empty" "\n";
  else
    return closest_match.match_report;
}

const Database::Entry* Database::Criteria::best (const Entry* a, const Entry* b) const
{
  if (!a && !b)
    throw Error (InvalidParam, "Database::Criteria::best", "both arguments are NULL");

  if (!a)
    return b;

  if (!b)
    return a;

  auto stat_a = dynamic_cast<const StaticEntry*> (a);
  if (!stat_a)
    return a;  // assume that interpolator is best
  
  auto stat_b = dynamic_cast<const StaticEntry*> (b);
  if (!stat_b)
    return b;  // assume that interpolator is best
  
  double a_diff = fabs( (stat_a->time - entry->time).in_minutes() );
  double b_diff = fabs( (stat_b->time - entry->time).in_minutes() );

  if (a_diff < b_diff)
    return a;
  else
    return b;
}

Database::Criteria
Database::Criteria::closest (const Criteria& a, const Criteria& b)
{
  if (Calibrator::verbose > 1)
    cerr << "Database::Criteria::closest \n"
      " A:" << a.match_count << "=" << a.match_report << "\n"
      " B:" << b.match_count << "=" << b.match_report << endl;

  if (a.match_count > b.match_count)
    return a;

  if (b.match_count > a.match_count)
    return b;

  if (a.diff_minutes < b.diff_minutes)
    return a;

  if (a.match_report.length() > b.match_report.length())
    return a;

  return b;
}

static Database::Criteria* default_criteria = 0;

//! Get the default matching criteria for PolnCal observations
Database::Criteria
Database::get_default_criteria ()
{
  if (!default_criteria)
    default_criteria = new Criteria;

  return *default_criteria;
}

void Database::set_default_criteria (const Criteria& criteria)
{
  if (!default_criteria)
    default_criteria = new Criteria;

  *default_criteria = criteria;
}




Database::Criteria
Database::criteria (const Pulsar::Archive* arch,
			    Signal::Source obsType) const
try {

  Criteria criteria = get_default_criteria();

  if (obsType == Signal::FluxCalOn ||
      obsType == Signal::FluxCalOff)
  {

    criteria.minutes_apart = long_time_scale;
    criteria.check_coordinates = false;
    criteria.check_instrument = false;
    criteria.set_sequence (Any);

  }
  else
    criteria.minutes_apart = short_time_scale;

  if (arch)
    criteria.entry = new StaticEntry (arch);
  else
    criteria.no_data ();

  criteria.entry->obsType = obsType;

  return criteria;

}
catch (Error& error)
{
  throw error += "Database::criteria Signal::Source";
}

//! Returns one Entry that matches the given parameters and is nearest in time.
Database::Criteria
Database::criteria (const Pulsar::Archive* arch, 
			     const Calibrator::Type* calType) const
try {

  Criteria criteria = get_default_criteria();

  if (calType->is_a<CalibratorTypes::Flux>() || 
      calType->is_a<CalibratorTypes::CompleteJones>())
  {
    criteria.minutes_apart = long_time_scale;

    // these solutions are global
    criteria.check_coordinates = false;

    // in principle, these solutions are indepenent of backend
    criteria.check_instrument = false;

    criteria.set_sequence(Any);
  }
  else
    criteria.minutes_apart = short_time_scale;

  if (arch)
    criteria.entry = new StaticEntry (arch);
  else
    criteria.no_data ();

  criteria.entry->obsType = Signal::Calibrator;
  criteria.entry->calType = calType;

  return criteria;
}
catch (Error& error)
{
  throw error += "Database::criteria Calibrator::Type";
}

template<class Container>
double get_centre_frequency (const Container* container, unsigned ichan)
{
  return container->get_centre_frequency (ichan);
}

double get_centre_frequency (const Pulsar::Archive* archive, unsigned ichan)
{
  return archive->get_Integration(0)->get_centre_frequency (ichan);
}

template<class Container, class TagAlong>
void remove_channels (const Pulsar::Archive* arch, 
                      Container* super, TagAlong* other = 0)
{
  Pulsar::ChannelSubsetMatch chan_match;

  if (Calibrator::verbose > 2)
    cerr << "Database::generatePolnCalibrator remove_channels" 
	 << "BW mismatch, trying channel truncation... " << endl;

  unsigned nremoved = 0;
  double chbw = fabs(arch->get_bandwidth() / arch->get_nchan());

  // Loop over polcal channels
  for (unsigned ichan=0; ichan<super->get_nchan(); ichan++)
  {
    DEBUG("remove_channels ichan=" << ichan << " nchan=" << super->get_nchan());

    double freq = get_centre_frequency (super, ichan);

    // Try to match them to archive channels
    try
    {
      chan_match.match_channel (arch->get_Integration(0), freq, 0.01*chbw);
    }
    catch (...)
    {
      DEBUG("no match for " << ichan << " of " << super->get_nchan());
      // If no match, delete it
      super->remove_chan(ichan,ichan);
      if (other)
        other->remove_chan(ichan,ichan);
      ichan--;
      nremoved++;
    }
  }

  if (Calibrator::verbose > 2) 
    cerr << "Database::generatePolnCalibrator remove_channels removed " 
	 << nremoved << " channels." << endl;

  // Test that the final numbers of channels match up
  if (super->get_nchan() != arch->get_nchan())
    throw Error (InvalidState, "remove_channels",
        "Channels could not be made to match up (final nsuper=%d, narch=%d)",
        super->get_nchan(), arch->get_nchan());
}

//! Removes channels from calarch if necessary
void match_channels (Pulsar::Archive* calarch, const Pulsar::Archive* arch)
{
  if ( calarch->get_bandwidth() == arch->get_bandwidth()
       || calarch->get_nchan() <= arch->get_nchan()
       || arch->get_nchan() == 0 )
    return;

  CalibratorStokes* calstokes = calarch->get<CalibratorStokes>();
  CalibratorExtension* ext = calarch->get<CalibratorExtension>();
  if (ext)
    remove_channels (arch, ext, calstokes);

  if (calarch->get_nsubint())
    remove_channels (arch, calarch, (Pulsar::Archive*) 0);
  
  calarch->set_centre_frequency( arch->get_centre_frequency() );
  calarch->set_bandwidth( arch->get_bandwidth() );
}


/*! This routine is given a pointer to a Pulsar::Archive. It scans the
  database for all FluxCal observations that were made within one month
  of the observation mid-time and uses these to construct a
  Pulsar::FluxCalibrator class which can then be used to perform a
  flux calibration of the original pulsar observation.
*/      
Pulsar::FluxCalibrator* 
Database::generateFluxCalibrator (Archive* arch, bool allow_raw) try {

  const Entry* match = best_match (criteria(arch, new CalibratorTypes::Flux));

  if (lastFluxCal.entry == match)
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generateFluxCalibrator using cached calibrator\n";
    return lastFluxCal.calibrator;
  }

  Reference::To<Archive> archive = Archive::load( get_filename(match) );
  match_channels(archive,arch);
  
  Reference::To<FluxCalibrator> fcal = new FluxCalibrator (archive);

  if (cache_last_cal)
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generateFluxCalibrator caching FluxCalibrator" << endl;
    lastFluxCal.cache (match, fcal);
  }

  return fcal.release();
}
catch (Error& error)
{  
  //if (Calibrator::verbose > 2)
    cerr << "Database::generateFluxCalibrator failure"
      " generating processed FluxCal\n" << error.get_message() << endl;
  
  if (allow_raw)
    return rawFluxCalibrator (arch);
  
  else
    throw error += "Database::generateFluxCalibrator";
}

Pulsar::FluxCalibrator* 
Database::rawFluxCalibrator (Pulsar::Archive* arch)
{
  vector<const Entry*> oncals;
  all_matching (criteria (arch, Signal::FluxCalOn), oncals);

  if (!oncals.size())
    throw Error (InvalidState, 
                 "Database::generateFluxCalibrator",
                 "no FluxCalOn observations found to match observation");

  vector<const Entry*> offcals;
  all_matching (criteria (arch, Signal::FluxCalOff), offcals);

  if (!offcals.size())
    throw Error (InvalidState,
                 "Database::generateFluxCalibrator",
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
Database::generatePolnCalibrator (Archive* arch,
				  const Calibrator::Type* type)
{
  if (!arch)
    throw Error (InvalidParam, "Database::generatePolnCalibrator",
		 "no Pulsar::Archive given");

  if (!type)
    throw Error (InvalidParam, "Database::generatePolnCalibrator",
                 "no Calibrator::Type given");

  if (Calibrator::verbose > 2)
    cerr << "Database::generatePolnCalibrator type="
	 << type->get_name() << endl;

  const Entry* entry = 0;

  //
  // unless a CompleteJones transformation is requested,
  // try loading a raw PolnCal observation
  //
  string polncal_match_report;

  if (! type->is_a<CalibratorTypes::CompleteJones>()) try
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator search for " 
	"Signal::PolnCal match" << endl;
    entry = best_match (criteria (arch, Signal::PolnCal));
  }
  catch (Error& error)
  {
    //
    // the Hybrid transformations require access to a raw PolnCal observation
    //
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator search for"
      "Signal::PolnCal failed. closest = " << get_closest_match_report();

    polncal_match_report = get_closest_match_report ();

    if (type->is_a<CalibratorTypes::Hybrid>())
    {
      error << "\n\tHybrid Calibrator requires raw PolnCal observation";
      throw error += "Database::generatePolnCalibrator";
    }
  }

  if (! type->is_a<CalibratorTypes::Hybrid>()) try
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator search for " 
	   << type->get_name() << " match" << endl;
    
    Criteria cal_criteria = criteria (arch, type);
    const Entry* cal_entry = best_match (cal_criteria);
    if (entry)
      entry = cal_criteria.best (entry, cal_entry);
    else
      entry = cal_entry;
  }
  catch (Error& error)
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator search for "
           << type->get_name() << " failed. closest = " << get_closest_match_report();

    if (!entry || entry->obsType == Signal::Unknown)
    {
      error << "\n\tneither raw nor processed calibrator archives found.\n"
               "\n\tRAW -- closest match: \n\n" << polncal_match_report <<
               "\n\tPROCESSED";
      throw error += "Database::generatePolnCalibrator";
    }
  }

  assert (entry != NULL);

  if (lastPolnCal.entry && lastPolnCal.entry->equals(entry))
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator using cached calibrator\n";
    return lastPolnCal.calibrator;
  }

  if (Calibrator::verbose > 2)
    cout << "Database::generatePolnCalibrator constructing from file "
	 << entry->filename << endl;

  Reference::To<Pulsar::Archive> polcalarch;
  polcalarch = Pulsar::Archive::load( get_filename(entry) );

  match_channels (polcalarch, arch);

  if (feed)
  {
    FeedExtension* feed_ext = polcalarch->getadd<FeedExtension>();
    feed_ext -> set_transformation ( feed->evaluate() );
  }

  if (Calibrator::verbose > 2)
  {
    if (entry->obsType == Signal::Calibrator)
      cerr << "CAL OF TYPE " <<  entry->calType->get_name() << endl;
    else
      cerr << "FILE OF TYPE " << Signal::Source2string (entry->obsType) << endl;
  }

  if (entry->obsType == Signal::Calibrator)
    // if a solved model, return the solution
    return new Pulsar::PolnCalibrator (polcalarch);

  // otherwise, construct a solution
  Reference::To<ReferenceCalibrator> ref_cal;
  ref_cal = ReferenceCalibrator::factory (type, polcalarch);
  
  if ( type->is_a<CalibratorTypes::Hybrid>() )
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator Hybrid" << endl;
    return generateHybridCalibrator (ref_cal, arch);
  }

  // store last calibrator, if we are caching
  if (cache_last_cal)
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generatePolnCalibrator caching PolnCalibrator" << endl;
    lastPolnCal.cache (entry, ref_cal);
  }

  return ref_cal.release();
}

Pulsar::HybridCalibrator* 
Database::generateHybridCalibrator (ReferenceCalibrator* arcal,
					    Archive* arch)
{
  if (!arch) throw Error (InvalidParam,
			  "Database::generateHybridCalibrator",
			  "no Pulsar::Archive given");
  if (!arcal) throw Error (InvalidParam,
			    "Database::generateHybridCalibrator",
			    "no Pulsar::ReferenceCalibrator given");
 
  if (Calibrator::verbose > 2)
    cerr << "Database::generateHybridCalibrator" << endl;

  const Entry* entry = 0;

  try
  {
    if (Calibrator::verbose > 2)
      cerr << "  Attempting to find a matching Phenomenological Model" << endl;

    entry = best_match (criteria(arch, new CalibratorTypes::CompleteJones));
  }
  catch (Error& error)
  {
    throw Error (InvalidState, "Database::generateHybridCalibrator",
		 "No complete parameterization (e.g. pcm output) found \n" +
                 get_closest_match_report ());
  }

  assert (entry != NULL);

  if (lastHybridCal.entry && lastHybridCal.entry->equals(entry))
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generateHybridCalibrator using cached calibrator\n";
    return lastHybridCal.calibrator;
  }

  Reference::To<Pulsar::HybridCalibrator> hybrid;

  try
  {
    Reference::To<Pulsar::Archive> polncalarch;
    polncalarch = Pulsar::Archive::load(get_filename(entry));

    match_channels (polncalarch, arch);

    Reference::To<Pulsar::PolnCalibrator> polncal;
    polncal = new Pulsar::PolnCalibrator (polncalarch);

    Reference::To<Pulsar::CalibratorStokes> calstokes;
    calstokes = polncalarch->get<CalibratorStokes>();

    hybrid = new Pulsar::HybridCalibrator (polncal);

    if (calstokes)
      hybrid->set_reference_input (calstokes);
    hybrid->set_reference_observation (arcal);
  }
  catch (Error& error)
  {
    throw error += "Database::generateHybridCalibrator";
  }

  if (cache_last_cal)
  {
    if (Calibrator::verbose > 2)
      cerr << "Database::generateHybridCalibrator caching HybridCalibrator" << endl;
    lastHybridCal.cache (entry, hybrid);
  }

  return hybrid.release();

}

//! Returns the full pathname of the Entry filename
string Database::get_filename (const Entry* entry) const
{
  if (!entry)
    return "N/A";

  if (entry->filename[0] == '/')
    return entry->filename;
  else
    return path + "/" + entry->filename;
}

//! Returns the full pathname of the Entry filename
string Database::get_path () const
{
  return path;
}

