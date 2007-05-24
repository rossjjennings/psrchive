/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZapInterpreter.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ChannelZapMedian.h"

#include "TextInterface.h"
#include "pairutil.h"

using namespace std;

Pulsar::ZapInterpreter::ZapInterpreter ()
{
  add_command 
    ( &ZapInterpreter::zap, 'z',
      "zap", "zap data using the specified algorithm",
      "usage: zap <median|chan|int> [indeces]\n"
      "  median            median smooth the passband and zap spikes \n"
      "  chan              zap channels specified by [indeces] \n"
      "  subint            zap integrations specified by [indeces] \n"
      "  such              zap only (subint,chan) specified in [indeces] \n");
}

Pulsar::ZapInterpreter::~ZapInterpreter ()
{
}

void parse_indeces (vector<unsigned>& indeces,
		    const vector<string>& arguments,
		    unsigned limit)
{
  // note that the first argument is the command name
  for (unsigned i=1; i<arguments.size(); i++)
    TextInterface::parse_indeces (indeces, "[" + arguments[i] + "]", limit);
}

template<typename T, typename U>
void parse_pairs (vector< pair<T,U> >& pairs,
		  const vector<string>& arguments,
		  T limit_first, const string& name_first,
		  U limit_second, const string& name_second)
{
  pairs.resize( arguments.size() - 1);

  for (unsigned i=0; i<pairs.size(); i++) {

    pairs[i] = fromstring< pair<T,U> > ( arguments[i+1] );
    
    if (pairs[i].first >= limit_first) {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_first << "=" << pairs[i].first 
	    << " >= n" << name_first << "=" << limit_first;
      throw error;
    }

    if (pairs[i].second >= limit_second) {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_second << "=" << pairs[i].second
	    << " >= n" << name_second << "=" << limit_second;
      throw error;
    }

    cerr << pairs[i] << endl;
  }
}

string Pulsar::ZapInterpreter::zap (const string& args)
try { 
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify zapping method");

  if (arguments[0] == "median") {
    if (!zap_median)
      zap_median = new ChannelZapMedian;
    
    zap_median->ChannelWeight::weight( get() );
  }

  else if (arguments[0] == "chan") {

    vector<unsigned> channels;
    parse_indeces (channels, arguments, get()->get_nchan());

    // zap selected channels in all sub-integrations
    unsigned nsubint = get()->get_nsubint();
    for (unsigned isub=0; isub<nsubint; isub++) {
      Integration* subint = get()->get_Integration(isub);
      for (unsigned i=0; i<channels.size(); i++)
	subint->set_weight( channels[i], 0.0 );
    }

  }

  else if (arguments[0] == "subint") {

    vector<unsigned> subints;
    parse_indeces (subints, arguments, get()->get_nsubint());

    // zap all channels in selected sub-integrations
    unsigned nchan = get()->get_nchan();
    for (unsigned i=0; i<subints.size(); i++) {
      Integration* subint = get()->get_Integration( subints[i] );
      for (unsigned ichan=0; ichan<nchan; ichan++)
	subint->set_weight( ichan, 0.0 );
    }

  }

  else if (arguments[0] == "such") {

    vector< pair<unsigned,unsigned> > pairs;
    parse_pairs (pairs, arguments,
		 get()->get_nsubint(), "subint",
		 get()->get_nchan(), "chan");

    for (unsigned i=0; i<pairs.size(); i++)
      get()->get_Integration(pairs[i].first)->set_weight(pairs[i].second,0.0);

  }

  else
    return response (Fail, "unrecognized zapping method '" + args + "'");

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


