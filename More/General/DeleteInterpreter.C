/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten and Fabian Jankowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeleteInterpreter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "TextInterface.h"
#include "pairutil.h"

using namespace std;

static string index_help (const string& cmd)
{
  return
    "usage: " + cmd + " iex1 [iex2 ...]\n"
    "  string iexN   unsigned index exression: [i|i1-i2|-iend|istart-]";
}

Pulsar::DeleteInterpreter::DeleteInterpreter ()
{
  add_command
    ( &DeleteInterpreter::chan,
      "chan", "delete specified channels",
      index_help("chan") );

  add_command
    ( &DeleteInterpreter::subint,
      "subint", "delete specified integrationss",
      index_help("subint") );

  add_command
    ( &DeleteInterpreter::freq,
      "freq", "delete frequency range(s)",
      "usage: freq < MHz0:MHz1 | >MHz | <MHz > \n"
      "  float pair <MHz0:MHz1> range of frequencies to delete \n"
      "  float [>MHz] delete everything above frequency \n"
      "  float [<MHz] delete everything below frequency \n");
}

Pulsar::DeleteInterpreter::~DeleteInterpreter ()
{
}

string Pulsar::DeleteInterpreter::empty ()
{
  return response (Fail, help());
}

void parse_indeces (vector<unsigned>& indeces,
		    const vector<string>& arguments,
		    unsigned limit)
{
  for (unsigned i=0; i<arguments.size(); i++)
    TextInterface::parse_indeces (indeces, "[" + arguments[i] + "]", limit);
}

string Pulsar::DeleteInterpreter::chan (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector<unsigned> channels;
  parse_indeces (channels, arguments, get()->get_nchan());

  // delete selected channels in all sub-integrations
  unsigned nsubint = get()->get_nsubint();
  for (unsigned isub=0; isub<nsubint; isub++) {
    Integration* subint = get()->get_Integration(isub);
    for (unsigned i=0; i<channels.size(); i++)
      subint->set_weight( channels[i], 0.0 );
  }
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


string Pulsar::DeleteInterpreter::subint (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector<unsigned> subints;
  parse_indeces (subints, arguments, get()->get_nsubint());

  std::sort (subints.begin(), subints.end(), std::greater<unsigned>());

  // delete all channels in selected sub-integrations
  unsigned nchan = get()->get_nchan();
  for (unsigned i=0; i<subints.size(); i++) {
    Integration* subint = get()->get_Integration( subints[i] );
    for (unsigned ichan=0; ichan<nchan; ichan++)
      subint->set_weight( ichan, 0.0 );
  }
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

#include "range.h"

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::DeleteInterpreter::freq (const string& args) try
{
  vector<string> arguments = setup (args);

  for (unsigned iarg=0; iarg < arguments.size(); iarg++)
  {
    range r = fromstring<range> (arguments[iarg]);

    Archive* archive = get();

    unsigned isub,  nsub = archive->get_nsubint();
    unsigned ichan, nchan = archive->get_nchan();

    for (isub=0; isub < nsub; isub++)
    {
      Integration* subint = archive->get_Integration (isub);
      for (ichan=0; ichan < nchan; ichan++)
	if (r.within( subint->get_centre_frequency(ichan) ))
	  subint->set_weight (ichan, 0.0);
    }
  }

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

