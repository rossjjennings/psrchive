/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZapInterpreter.h"
#include "Pulsar/ChannelZapMedian.h"

using namespace std;

Pulsar::ZapInterpreter::ZapInterpreter ()
{
  add_command 
    ( &ZapInterpreter::zap, 'z',
      "zap", "zap data using the specified algorithm",
      "usage: zap <median> \n"
      "  median            median smooth the passband and zap spikes \n" );
}

Pulsar::ZapInterpreter::~ZapInterpreter ()
{
}

string Pulsar::ZapInterpreter::zap (const string& args)
try { 
  vector<string> arguments = setup (args);

  if (!arguments.size())
    return response (Fail, "please specify zapping algorithm");

  if (arguments[0] == "median") {
    if (!zap_median)
      zap_median = new ChannelZapMedian;
    
    zap_median->ChannelWeight::weight( get() );
  }

  else
    return response (Fail, "unrecognized zapping algorithm '" + args + "'");

  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


