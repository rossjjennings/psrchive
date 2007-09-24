/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileInterpreter.h"
#include "Pulsar/ArchiveTemplates.h"

#include "Pulsar/Accumulate.h"
#include "Pulsar/Differentiate.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"

using namespace std;

Pulsar::ProfileInterpreter::ProfileInterpreter ()
{
  add_command 
    ( &ProfileInterpreter::mean,
      "mean", "form the mean smoothed profile",
      "usage: mean <duty_cycle | bins>\n" );

  add_command 
    ( &ProfileInterpreter::median,
      "median", "form the median smoothed profile",
      "usage: median <duty_cycle | bins>\n" );

  add_command 
    ( &ProfileInterpreter::cumulative,
      "cumulative", "form the cumulative profile",
      "usage: cumulative\n" );

  add_command 
    ( &ProfileInterpreter::difference,
      "difference", "form the difference profile",
      "usage: difference \n" );
}

Pulsar::ProfileInterpreter::~ProfileInterpreter ()
{
}


string Pulsar::ProfileInterpreter::cumulative (const string& args) try
{
  foreach (get(), new Accumulate);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::difference (const string& args) try
{ 
  foreach (get(), new Differentiate);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::mean (const string& args) try
{ 
  foreach (get(), new SmoothMean);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::median (const string& args) try
{ 
  foreach (get(), new SmoothMedian);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::empty ()
{
  return response (Fail, help());
}


