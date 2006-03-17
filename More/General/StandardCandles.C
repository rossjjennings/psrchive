/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxCalibratorDatabase.h"
#include "Error.h"
#include "string_utils.h"

#include <fstream>
#include <algorithm>

#include <math.h>
#include <ctype.h>

bool Pulsar::FluxCalibratorDatabase::verbose = false;

std::string
Pulsar::FluxCalibratorDatabase::default_filename = getenv ("PSRHOME") 
  + std::string ("/runtime/psrchive/fluxcal.cfg");

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
}

//! Destructor
Pulsar::FluxCalibratorDatabase::Entry::~Entry ()
{
  
}

// load from ascii string
void Pulsar::FluxCalibratorDatabase::Entry::load (const string& str) 
{
  string temp = str;

  if( h_frontchomp(temp,'&') ){
    // Load-mode type II
    // log(S) = a_0 + a_1*log(f) + a_2*(log(f))^2 + a_3*(log(f))^4 + ...
    vector<string> words = stringdecimate(temp," \t");

    if( words.size() < 3 )
      throw Error(InvalidState,"Pulsar::FluxCalibratorDatabase::Entry::load",
		  "Couldn't parse spectral coefficients as line '%s' didn't have enough words in it",
		  str.c_str());

    source_name.push_back( words[0] );

    for( unsigned i=1; i<words.size(); i++)
      spectral_coeffs.push_back( convert_string<double>(words[i]) );
    return;
  }

  source_name.push_back( stringtok (&temp, " \t\n") );

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
bool Pulsar::FluxCalibratorDatabase::Entry::matches (const string& name) const
{
  for (unsigned iname=0; iname < source_name.size(); iname ++)
    if (close_enough(source_name[iname], name))
      return true;

  return false;
}

double Pulsar::FluxCalibratorDatabase::Entry::get_flux_mJy (double MHz)
{
  if( !spectral_coeffs.empty() ){
    double ret = spectral_coeffs[0];
    double log_freq = log(MHz)/log(10.0);

    if( verbose )
      fprintf(stderr,"Pulsar::FluxCalibratorDatabase::Entry::get_flux_mJy Got f=%f MHz and log(f)=%f and ret=%f\n",
	      MHz, log_freq, ret);

    for( unsigned i=1; i<spectral_coeffs.size(); i++)
      ret += spectral_coeffs[i]*pow(log_freq,double(i));

    return 1000.0*pow(10.0,ret);
  }

  return pow (MHz/reference_frequency, -spectral_index) * reference_flux * 1e3;
}

Pulsar::FluxCalibratorDatabase::FluxCalibratorDatabase ()
{
  if (verbose)
    cerr << "Pulsar::FluxCalibratorDatabase load " << default_filename << endl;

  load (default_filename);
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

    if (line.substr(0,3) == "aka") {

      if (entries.size() == 0)
	throw Error (InvalidState, "Pulsar::FluxCalibratorDatabase::load",
		     "cannot add aliases before entries");

      // take off the aka
      string name = stringtok (&line, " \t\n");
      // get the name
      name = stringtok (&line, " \t\n");

      if (verbose)
	cerr << "Pulsar::FluxCalibratorDatabase::load alias: " 
	     << entries.back().source_name[0] << " = " << name << endl;

      entries.back().source_name.push_back (name);

    }
    else {
      entries.push_back( Entry(line) );

      if (verbose)
	cerr << "Pulsar::FluxCalibratorDatabase::load added: " 
	     << entries.back().source_name[0] << endl;

    }

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
Pulsar::FluxCalibratorDatabase::match (const string& source, double MHz) const
{
  Entry best_match;
  Entry candidate;

  for (unsigned ie=0; ie<entries.size(); ie++) {
    if (entries[ie].matches(source)) {
      // Added this next "if" to ensure that a match is returned
      // even if there is only one appropriate entry. AWH 28/2/05
      if (best_match.reference_frequency == 0)
	best_match = entries[ie];
      double diff = fabs(candidate.reference_frequency - MHz);
      double best_diff = fabs(best_match.reference_frequency - MHz);
      if (diff < best_diff)
	best_match = entries[ie];
    }
  }

  if (best_match.reference_frequency == 0)
    throw Error (InvalidParam, "Pulsar::FluxCalibratorDatabase::match",
		 "No match for source=" + source);

  return best_match;
}
