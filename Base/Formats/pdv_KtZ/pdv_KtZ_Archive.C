/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/pdv_KtZ_Archive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Pulsar.h"

#include "strutil.h"

// #define _DEBUG 1
#include "debug.h"

#include <fstream>

using namespace std;
using namespace Pulsar;

void Pulsar::pdv_KtZ_Archive::init ()
{
  integration_length = 0.0;
  period = 1.0;
}

Pulsar::pdv_KtZ_Archive::pdv_KtZ_Archive()
{
  init ();
}

Pulsar::pdv_KtZ_Archive::~pdv_KtZ_Archive()
{
  // destroy any pdv_KtZ_Archive resources
}

Pulsar::pdv_KtZ_Archive::pdv_KtZ_Archive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::pdv_KtZ_Archive::pdv_KtZ_Archive (const pdv_KtZ_Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive construct copy pdv_KtZ_Archive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::pdv_KtZ_Archive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive::copy dynamic cast call" << endl;

  const pdv_KtZ_Archive* like_me = dynamic_cast<const pdv_KtZ_Archive*>(&archive);
  if (!like_me)
    return;

  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive::copy another pdv_KtZ_Archive" << endl;

  integration_length = like_me -> integration_length;
}

Pulsar::pdv_KtZ_Archive* Pulsar::pdv_KtZ_Archive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::pdv_KtZ_Archive::clone" << endl;
  return new pdv_KtZ_Archive (*this);
}

template<typename T>
bool parse (std::string& line, const std::string& key, T& value)
{
  DEBUG( "parse line='" << line << "'" );
  DEBUG( "parse key='" << key << "'" );
  
  string before, after;
  string_split(line, before, after, key);

  DEBUG( "1: parse before='" << before << "'" );
  DEBUG( "1: parse after='" << after << "'" );
  
  // keyword not found
  if (after.empty())
  {
    DEBUG("after is empty");
    return false;
  }
  
  const string whitespace = " \t\n";

  // remove preceding whitespace
  string_split_on_any (after, before, after, whitespace);

  DEBUG( "2: parse before='" << before << "'" );
  DEBUG( "2: parse after='" << after << "'" );
  
  if (before.empty())
  {
    string tmp_after;
    string_split_on_any (after, before, tmp_after, whitespace);
    DEBUG( "2a: parse before='" << before << "'" );
    DEBUG( "2a: parse after='" << tmp_after << "'" );

    // at the end of the line, there is no after
    if (tmp_after == "")
      before = after;

    after = tmp_after;

    DEBUG( "2b: parse before='" << before << "'" );
    DEBUG( "2b: parse after='" << after << "'" );
  }
  // value not found
  if (before.empty())
  {
    DEBUG( "before is empty" );
    return false;
  }
  
  DEBUG( "parse value='" << before << "'" );

  line = after;

  try {
    value = fromstring<T> (before);
  }
  catch (...)
    {
      return false;
    }
  
  DEBUG( "parse value=" << value );
  
  return true;
}

void Pulsar::pdv_KtZ_Archive::load_header (const char* filename)
{
  ifstream is (filename);
  if (!is)
    throw Error (FailedSys, "Pulsar::pdv_KtZ_Archive::load_header",
		 "ifstream(%s)", filename);

  // check for the hash in pdv header
  if (is.peek() != '#')
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "first line of '%s' does not start with '#'", filename);

  // get the header line
  string line;
  getline (is, line);

  // //////////////////////////////////////////////////////////////////////
  //
  // source name
  //
  string source;
  if (!parse (line, "Src:", source))
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "could not parse source from '%s'", filename);
  set_source (source);

  // //////////////////////////////////////////////////////////////////////
  //
  // number of sub-integrations
  //
  unsigned nsubint = 0;
  if (!parse (line, "Nsub:", nsubint))
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "could not parse nsubint from '%s'", filename);
  
  // //////////////////////////////////////////////////////////////////////
  //
  // number of frequency channels
  //
  unsigned nchan = 0;
  if (!parse (line, "Nch:", nchan))
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "could not parse nchan from '%s'", filename);
  
  // //////////////////////////////////////////////////////////////////////
  //
  // number of polarizations
  //
  unsigned npol = 0;
  if (!parse (line, "Npol:", npol))
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "could not parse npol from '%s'", filename);

  if (npol == 4)
    set_state (Signal::Stokes);
  else if (npol == 2)
    set_state (Signal::PPQQ);
  else if (npol == 1)
    set_state (Signal::Intensity);
  else
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "unsupported npol = %u", npol);
  
  // //////////////////////////////////////////////////////////////////////
  //
  // number of phase bins
  //
  unsigned nbin = 0;
  if (!parse (line, "Nbin:", nbin))
    throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load_header",
		 "could not parse nbin from '%s'", filename);

  resize (nsubint, npol, nchan, nbin);

  unsigned isub_offset = 0;
  unsigned ichan_offset = 0;
  unsigned ibin_offset = 0;
  bool first_line = true;
  
  for (unsigned isub=0; isub < get_nsubint(); isub++)
  { 
    BasicIntegration* subint;
    subint = dynamic_cast<BasicIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "pdv_KtZ_Archive::load",
		   "Integration[%d] is not a BasicIntegration", isub);
        
    for (unsigned ichan=0; ichan < get_nchan(); ichan++)
    {
      // check for the pdv sub-header
      if (is.peek() == '#')
      {
	// get the sub-header line
	getline (is, line);

	// ////////////////////////////////////////////////////////////////////
	//
	// epoch
	//
	if (!parse (line, "MJD(mid):", epoch))
	  warning << "Pulsar::pdv_KtZ_Archive::load_header"
	    " could not parse MJD" << endl;
	
	// ////////////////////////////////////////////////////////////////////
	//
	// period
	//
	if (!parse (line, "P:", period))
	  warning << "Pulsar::pdv_KtZ_Archive::load_header"
	    " could not parse period" << endl;

	DEBUG( "After P: remaining line='" << line << "'");

	// ////////////////////////////////////////////////////////////////////
	//
	// integration length
	//
	if (!parse (line, "Tsub:", integration_length))
	  warning << "Pulsar::pdv_KtZ_Archive::load_header"
	    " could not parse integration_length" << endl;

	DEBUG( "After Tsub: remaining line='" << line << "'" );

	// ////////////////////////////////////////////////////////////////////
	//
	// frequency
	//
	if (!parse (line, "Freq:", centre_frequency))
	  warning << "Pulsar::pdv_KtZ_Archive::load_header"
	    " could not parse centre frequency" << endl;

	DEBUG( "After Freq: remaining line='" << line << "'" );
	
	// ////////////////////////////////////////////////////////////////////
	//
	// bandwidth
	//
	if (!parse (line, "BW:", bandwidth))
	  warning << "Pulsar::pdv_KtZ_Archive::load_header"
	    " could not parse bandwidth" << endl;
      }
      
      subint->set_folding_period (period);
      subint->set_duration (integration_length);
      subint->set_epoch (epoch);
      subint->set_centre_frequency (ichan, centre_frequency);
    
      if (Profile::no_amps)
	break;
      
      for (unsigned ibin=0; ibin < get_nbin(); ibin++)
      {
	if (verbose > 2)
	  cerr << "Pulsar::pdv_KtZ_Archive::load ibin=" << ibin << endl;

	unsigned jsub, jchan, jbin;
	is >> jsub >> jchan >> jbin;

	if (is.fail())
	  throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load",
		       "index read failed");

	if (first_line)
	{
	  isub_offset = jsub;
	  ichan_offset = jchan;
	  ibin_offset = jbin;
	}

	first_line = false;
	
        if (jsub != isub + isub_offset)
	  throw Error (InvalidState, "pdv_KtZ_Archive::load",
		       "parsed isub=%u != expected isub=%u",
		       jsub, isub + isub_offset);
	
	if (jchan != ichan + ichan_offset)
	  throw Error (InvalidState, "pdv_KtZ_Archive::load",
		       "parsed ichan=%u != expected ichan=%u",
		       jchan, ichan + ichan_offset);
	
	if (jbin != ibin + ibin_offset)
	  throw Error (InvalidState, "pdv_KtZ_Archive::load",
		       "parsed ibin=%u != expected ibin=%u",
		       jbin, ibin + ibin_offset);

	for (unsigned ipol=0; ipol < get_npol(); ipol++)
	{
	  double value;
	  is >> value;

	  subint->get_Profile(ipol,ichan) -> get_amps()[ibin] = value;
	}

	if (is.fail())
	  throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load",
		       "data read failed");
	
	// clear the remainder of the line
	getline (is, line);

	if (is.fail())
	  throw Error (InvalidParam, "Pulsar::pdv_KtZ_Archive::load",
		       "clear read failed");
      } // for each bin
    } // for each channel
  } // for each subint
}


Pulsar::Integration*
Pulsar::pdv_KtZ_Archive::load_Integration (const char* filename, unsigned subint)
{
  throw Error (InvalidState, "Pulsar::pdv_KtZ_Archive::load_Integration",
	       "not implemented");
}

void Pulsar::pdv_KtZ_Archive::unload_file (const char* filename) const
{
  throw Error (InvalidState, "Pulsar::pdv_KtZ_Archive::unload_file",
	       "not implemented");
}

string Pulsar::pdv_KtZ_Archive::Agent::get_description () 
{
  return "pdv -KtZ output parser";
}


bool Pulsar::pdv_KtZ_Archive::Agent::advocate (const char* filename) try
{
  pdv_KtZ_Archive archive;
  archive.load_header (filename);
  return true;
}
catch (Error& e) 
{
  if (Archive::verbose > 2)
    cerr << "pdv_KtZ_ load failed due to: " << e << endl;

  return false;
}
catch (std::exception& e)
{
  return false;
}
