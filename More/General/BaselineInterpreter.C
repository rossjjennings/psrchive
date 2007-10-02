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
    ( &BaselineInterpreter::normal,
      "normal", "install GaussianBaseline algorithm",
      "usage: normal \n" );

  add_command 
    ( &BaselineInterpreter::minimum,
      "minimum", "install BaselineWindow algorithm (default)",
      "usage: minimum \n" );

}

string Pulsar::BaselineInterpreter::normal (const string& args) try
{
  if (!normal_functor)
    normal_functor.set( new GaussianBaseline, &BaselineEstimator::baseline );

  Profile::baseline_strategy = normal_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::BaselineInterpreter::minimum (const string& args) try
{ 
  if (!minimum_functor)
    minimum_functor.set( new BaselineWindow, &BaselineEstimator::baseline );

  Profile::baseline_strategy = minimum_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}
    
string Pulsar::BaselineInterpreter::empty ()
{ 
  if (Profile::baseline_strategy == normal_functor)
    return "normal";

  if (Profile::baseline_strategy == minimum_functor)
    return "minimum";

  return "unknown";
}




