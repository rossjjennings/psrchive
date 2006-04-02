/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxCalibratorDatabase.h"
#include "Pulsar/Config.h"

#include "Error.h"
#include "string_utils.h"

#include <fstream>
#include <algorithm>

#include <math.h>
#include <ctype.h>

using namespace std;

bool Pulsar::FluxCalibrator::Database::verbose = false;

static std::string get_default ()
{
  char* psrhome = getenv ("PSRHOME");

  if (psrhome)
    return psrhome + std::string ("/runtime/psrchive/fluxcal.cfg");
  else
    return "fluxcal.cfg";
}

std::string
Pulsar::FluxCalibrator::Database::default_filename 
= Pulsar::config.get<std::string> ("fluxcal::database", get_default());

float Pulsar::FluxCalibrator::Database::on_radius
= Pulsar::config.get<float> ("fluxcal:on_radius", 0.25);

float Pulsar::FluxCalibrator::Database::off_radius
= Pulsar::config.get<float> ("fluxcal:off_radius", 2.5);

// //////////////////////////////////////////////////////////////////////
//
// Pulsar::FluxCalibrator::Database::Entry
//
// filename type posn MJD bandwidth cfrequency nchan instrument
//

//! Initialise all variables
void Pulsar::FluxCalibrator::Database::Entry::init ()
{
  reference_frequency = 0.0;
  reference_flux = 0.0;
  spectral_index = 0.0;
}

//! Destructor
Pulsar::FluxCalibrator::Database::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::FluxCalibrator::Database::Entry::load (const string& str) 
{
  const string whitespace = " \t\n";

  string temp = str;

  if( h_frontchomp(temp,'&') ){
    // Load-mode type II
    // log(S) = a_0 + a_1*log(f) + a_2*(log(f))^2 + a_3*(log(f))^4 + ...
    vector<string> words = stringdecimate(temp, whitespace);

    if( words.size() < 5 )
      throw Error(InvalidState,"Pulsar::FluxCalibrator::Database::Entry::load",
		  "Couldn't parse spectral coefficients as line '%s' didn't have enough words in it",
		  str.c_str());

    source_name.push_back( words[0] );

    string coordstr = words[1] + " " + words[2];
    position = sky_coord (coordstr.c_str());

    for( unsigned i=3; i<words.size(); i++)
      spectral_coeffs.push_back( convert_string<double>(words[i]) );
    return;
  }

  // /////////////////////////////////////////////////////////////////
  // NAME
  source_name.push_back( stringtok (&temp, whitespace) );

  // /////////////////////////////////////////////////////////////////
  // RA DEC
  string coordstr = stringtok (&temp, whitespace);
  coordstr += " " + stringtok (&temp, whitespace);
  position = sky_coord (coordstr.c_str());

  int s = sscanf (temp.c_str(), "%lf %lf %lf",
		  &reference_frequency,
		  &reference_flux, 
		  &spectral_index);

  if (s != 3)
    throw Error (FailedSys, "Pulsar::FluxCalibrator::Database::Entry::load",
                 "sscanf(%s) != 3", str.c_str());
}

// unload to a string
void Pulsar::FluxCalibrator::Database::Entry::unload (string& str)
{
  if( !spectral_coeffs.empty() ){
    str = '&' + source_name[0];

    for( unsigned i=0; i<spectral_coeffs.size(); i++)
      str += ' ' + make_string(spectral_coeffs[i],6);
  }
  else
    str = source_name[0] + stringprintf (" %lf %lf %lf",
					 reference_frequency,
					 reference_flux, 
					 spectral_index);

  for (unsigned iname=1; iname < source_name.size(); iname ++)
    str += "\naka " + source_name[iname];
}

bool close_enough (string A, string B)
{
  std::transform (A.begin(), A.end(), A.begin(), ::tolower);
  std::transform (B.begin(), B.end(), B.begin(), ::tolower);

  return A.find(B)!=string::npos || B.find(A)!=string::npos;
}

//! return true if the source name matches
bool Pulsar::FluxCalibrator::Database::Entry::matches (const string& name) const
{
  for (unsigned iname=0; iname < source_name.size(); iname ++)
    if (close_enough(source_name[iname], name))
      return true;

  return false;
}

double Pulsar::FluxCalibrator::Database::Entry::get_flux_mJy (double MHz)
{
  if( !spectral_coeffs.empty() ){
    double ret = spectral_coeffs[0];
    double log_freq = log(MHz)/log(10.0);

    if( verbose )
      fprintf(stderr,"Pulsar::FluxCalibrator::Database::Entry::get_flux_mJy Got f=%f MHz and log(f)=%f and ret=%f\n",
	      MHz, log_freq, ret);

    for( unsigned i=1; i<spectral_coeffs.size(); i++)
      ret += spectral_coeffs[i]*pow(log_freq,double(i));

    return 1000.0*pow(10.0,ret);
  }

  return pow (MHz/reference_frequency, -spectral_index) * reference_flux * 1e3;
}

Pulsar::FluxCalibrator::Database::Database ()
{
  if (verbose)
    cerr << "Pulsar::FluxCalibrator::Database load " << default_filename << endl;

  load (default_filename);
}

Pulsar::FluxCalibrator::Database::~Database ()
{
}

Pulsar::FluxCalibrator::Database::Database (const std::string& f)
{
  if (verbose)
    cerr << "Pulsar::FluxCalibrator::Database load " << f << endl;

  load (f);
}

//! Loads an entire database from a file
void Pulsar::FluxCalibrator::Database::load (const std::string& filename)
{

  std::ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "Pulsar::FluxCalibrator::Database::load",
		 "ifstream (" + filename + ")");

  string line;

  while (!input.eof()) {

    getline (input, line);

    line = stringtok (&line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    if (verbose)
      cerr << "Pulsar::FluxCalibrator::Database::load '"<< line << "'" << endl;

    if (line.substr(0,3) == "aka") {

      if (entries.size() == 0)
	throw Error (InvalidState, "Pulsar::FluxCalibrator::Database::load",
		     "cannot add aliases before entries");

      // take off the aka
      string name = stringtok (&line, " \t\n");
      // get the name
      name = stringtok (&line, " \t\n");

      if (verbose)
	cerr << "Pulsar::FluxCalibrator::Database::load alias: " 
	     << entries.back().source_name[0] << " = " << name << endl;

      entries.back().source_name.push_back (name);

    }
    else {
      entries.push_back( Entry(line) );

      if (verbose)
	cerr << "Pulsar::FluxCalibrator::Database::load added: " 
	     << entries.back().source_name[0] << endl;

    }

  }

}

//! Unloads entire database to file
void Pulsar::FluxCalibrator::Database::unload (const std::string& filename)
{
  FILE* fptr = fopen (filename.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Pulsar::FluxCalibrator::Database::unload" 
		 "fopen (" + filename + ")");
  
  string out;
  for (unsigned ie=0; ie<entries.size(); ie++) {
    entries[ie].unload(out);
    fprintf (fptr, "%s\n", out.c_str());
  }
  fclose (fptr);
}

Pulsar::FluxCalibrator::Database::Entry
Pulsar::FluxCalibrator::Database::match (const string& source, double MHz) const
{
  Entry best_match;

  for (unsigned ie=0; ie<entries.size(); ie++) {
    if (entries[ie].matches(source)) {
      if (best_match.reference_frequency == 0)
	best_match = entries[ie];
      else {
	double diff = fabs(entries[ie].reference_frequency - MHz);
	double best_diff = fabs(best_match.reference_frequency - MHz);
	if (diff < best_diff)
	  best_match = entries[ie];
      }
    }
  }

  if (best_match.reference_frequency == 0)
    throw Error (InvalidParam, "Pulsar::FluxCalibrator::Database::match",
		 "No match for source=" + source);

  return best_match;
}

Signal::Source
Pulsar::FluxCalibrator::Database::guess (string& name, sky_coord& p) const
{
  double closest = 0.0;
  name = "unknown";

  for (unsigned i=0; i<entries.size(); i++) {

    double separation = entries[i].position.angularSeparation(p).getDegrees();

    if (closest == 0.0 || separation < closest) {
      closest = separation;
      name = entries[i].source_name[0];
    }

  }

  if (name == "unknown")
    return Signal::Unknown;

  if (closest <= on_radius)
    return Signal::FluxCalOn;

  if (closest <= off_radius)
    return Signal::FluxCalOff;

  name = "unknown";
  return Signal::Unknown;
}
