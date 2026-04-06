/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// #define _DEBUG 1

#include "Pulsar/GeneratorInterpreter.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

using namespace std;

Pulsar::Generator::Interpreter::Interpreter ()
{
  DEBUG("Pulsar::Generator::Interpreter ctor this=" << this);

  current = "polyco";

  // configure CommandParser to throw exception on parse error
  abort = true;

  add_command 
    ( &Generator::Interpreter::polyco,
      "polyco", "use tempo polyco by default",
      "usage: polyco \n" );

  add_command
    ( &Generator::Interpreter::tempo2,
      "tempo2", "use tempo2 predictor by default",
      "usage: tempo2 \n" );
}

Pulsar::Generator::Interpreter::~Interpreter ()
{
  DEBUG("Pulsar::Generator::Interpreter dtor this=" << this);
}

string Pulsar::Generator::Interpreter::polyco (const string& args) try
{
  DEBUG("Pulsar::Generator::Interpreter::polyco");

  get_default_generator().set_value( new Tempo::Predict );
  current = "polyco";
  return "";
}
catch (Error& error)
{
  return error.get_message();
}

string Pulsar::Generator::Interpreter::tempo2 (const string& args) try
{ 
  DEBUG("Pulsar::Generator::Interpreter::tempo2");

#ifdef HAVE_TEMPO2
  get_default_generator().set_value( new Tempo2::Generator );
  current = "tempo2";
#else
  cerr << "Pulsar::Generator::Interpreter::tempo2 TEMPO2 is not available" << endl;
  exit(-1);
#endif

  return "";
}
catch (Error& error) {
  return error.get_message();
}

    
string Pulsar::Generator::Interpreter::empty ()
{
  DEBUG("Pulsar::Generator::Interpreter::empty current=" << current);

  // ensure that the Configuration::Parameter is loaded
  get_default_generator().get_value();

  return current;
}

Configuration::Parameter<Pulsar::Generator*>&
Pulsar::Generator::Interpreter::get_option ()
{
  [[maybe_unused]] Generator* generator = get_default_generator().get_value();

  DEBUG("Pulsar::Generator::Interpreter::get_option generator=" << generator);
  return get_default_generator();
}

