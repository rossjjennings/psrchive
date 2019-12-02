/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourthMomentInterpreter.h"
#include "Pulsar/FourthMomentStats.h"

using namespace std;

Pulsar::FourthMomentInterpreter::FourthMomentInterpreter ()
{
  add_command 
    ( &FourthMomentInterpreter::debias,
      "debias", "remove the bias due to noise" );
}

Pulsar::FourthMomentInterpreter::~FourthMomentInterpreter ()
{
}

string Pulsar::FourthMomentInterpreter::debias (const string& args) try
{
  FourthMomentStats::debias (get());  
  return response (Good);
}
catch (Error& error)
{
  cerr << error << endl;
  return response (Fail, error.get_message());
}

