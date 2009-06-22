/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Pulsar/GeneratorInterpreter.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

Pulsar::Generator::Interpreter::Interpreter ()
{
  add_command 
    ( &Generator::Interpreter::polyco,
      "polyco", "use tempo polyco by default",
      "usage: polyco \n" );

#ifdef HAVE_TEMPO2
  add_command 
    ( &Generator::Interpreter::tempo2,
      "tempo2", "use tempo2 predictor by default",
      "usage: tempo2 \n" );
#endif
}


string Pulsar::Generator::Interpreter::polyco (const string& args) try
{
  default_generator = new Tempo::Predict;
  current = "polyco";
  return "";
}
catch (Error& error)
{
  return error.get_message();
}

#ifdef HAVE_TEMPO2

string Pulsar::Generator::Interpreter::tempo2 (const string& args) try
{ 
  default_generator = new Tempo2::Generator;
  current = "tempo2";
  return "";
}
catch (Error& error) {
  return error.get_message();
}

#endif
    
string Pulsar::Generator::Interpreter::empty ()
{ 
  return current;
}

