#include "Pulsar/FluxCalibratorDatabase.h"
#include "Error.h"
#include "string_utils.h"

#include <fstream>

bool Pulsar::FluxCalibratorDatabase::verbose = false;


// //////////////////////////////////////////////////////////////////////
//
// Pulsar::FluxCalibratorDatabase::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Pulsar::FluxCalibratorDatabase::Entry::init ()
{
  reference_frequency = 0.0;
  reference_flux = 0.0;
  spectral_index = 0.0;

  source_name = "unset";
}

//! Destructor
Pulsar::FluxCalibratorDatabase::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::FluxCalibratorDatabase::Entry::load (const string& str) 
{
  string temp = str;
  source_name = stringtok (&temp, " \t\n");

  int s = sscanf (temp.c_str(), "%lf %lf %lf",
		  &reference_frequency,
		  &reference_flux, 
		  &spectral_index);

  if (s != 3)
    throw Error (FailedSys, "Pulsar::FluxCalibratorDatabase::Entry::load",
                 "sscanf(%s) != 3", str.c_str());
}

// unload to a string
void Pulsar::FluxCalibratorDatabase::Entry::unload (string& str)
{
  str = source_name + stringprintf (" %lf %lf %lf",
				    reference_frequency,
				    reference_flux, 
				    spectral_index);
}


Pulsar::FluxCalibratorDatabase::FluxCalibratorDatabase ()
{
}

Pulsar::FluxCalibratorDatabase::~FluxCalibratorDatabase ()
{
}

Pulsar::FluxCalibratorDatabase::FluxCalibratorDatabase (const std::string& f)
{
  if (verbose)
    cerr << "Pulsar::FluxCalibratorDatabase load " << f << endl;

  load (f);
}


//! Loads an entire database from a file
void Pulsar::FluxCalibratorDatabase::load (const std::string& filename)
{

  std::ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Pulsar::FluxCalibratorDatabase::load",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof()) {

    getline (input, line);

    line = stringtok (&line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    if (verbose)
      cerr << "Pulsar::FluxCalibratorDatabase::load '"<< line << "'" << endl;

    entries.push_back( Entry(line) );

  }

}

//! Unloads entire database to file
void Pulsar::FluxCalibratorDatabase::unload (const std::string& filename)
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::FluxCalibratorDatabase::unload" 
		 "fopen (" + filename + ")");
  
  string out;
  for (unsigned ie=0; ie<entries.size(); ie++) {
    entries[ie].unload(out);
    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);
}

Pulsar::FluxCalibratorDatabase::Entry
Pulsar::FluxCalibratorDatabase::match (const std::string& source, double MHz)
{
  Entry best_match;

  for (unsigned ie=0; ie<entries.size(); ie++)
    if (entries[ie].source_name == source) {
      double diff = fabs(entries[ie].reference_frequency - MHz);
      double best_diff = fabs(best_match.referefreq - MHz);
      if (diff < best_diff)
	best_match = entries[ie];
    }

  if (best_match.reference_frequency == 0)
    throw Error (InvalidParam, "Pulsar::FluxCalibratorDatabase::match",
		 "No match for source=" + source);

  return best_match;
}
