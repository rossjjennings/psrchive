/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineInterpreter.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::BaselineInterpreter::BaselineInterpreter ()
{
  add_command 
    ( &BaselineInterpreter::gaussian,
      "normal", "install GaussianBaseline algorithm",
      "usage: normal\n" );

  add_command 
    ( &BaselineInterpreter::minimum,
      "min", "install BaselineWindow algorithm (default)",
      "usage: min \n" );
}

Pulsar::BaselineInterpreter::~BaselineInterpreter ()
{
}


string Pulsar::BaselineInterpreter::gaussian (const string& args) try
{
  Profile::baseline_strategy = Functor< PhaseWeight* (const Profile*) >
    ( new GaussianBaseline, &BaselineEstimator::baseline );
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::BaselineInterpreter::minimum (const string& args) try
{ 
  Profile::baseline_strategy = Functor< PhaseWeight* (const Profile*) >
    ( new BaselineWindow, &BaselineEstimator::baseline );
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


string Pulsar::BaselineInterpreter::empty ()
{
  return response (Fail, help());
}


