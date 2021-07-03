//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/InterpreterExtension.h

#ifndef __Pulsar_Interpreter_Extension_h
#define __Pulsar_Interpreter_Extension_h

#include "Pulsar/Interpreter.h"

namespace Pulsar {

  class Interpreter::Extension : public CommandParser {

  public:

    enum Status { Good, Warn, Fail };

    //! get the current Archive
    Archive* get ()
    { return interpreter->get(); }

    //! get the named Archive
    Archive* getmap (const std::string& name)
    { return interpreter->getmap(name); }

    std::vector<std::string> setup (const std::string& text, bool expand = true)
    { return interpreter->setup (text, expand); }

    //! Parses arguments as a single instance of T
    template<typename T> T setup (const std::string& args)
    { return interpreter->setup<T> (args); }

    //! Parses arguments as an optional single instance of T
    template<typename T> T setup (const std::string& args, T default_value)
    { return interpreter->setup<T> (args, default_value); }

    std::string response (Status s, const std::string& text = "")
      { return interpreter->response ((Interpreter::Status)s, text); }

    std::string response (const Error& error)
      { return interpreter->response (error); }

    TextInterface::Parser* get_interface ()
      { return interpreter->get_interface (); }

  protected:

    friend class Interpreter;

    //! The parent Interpreter
    Reference::To<Interpreter, false> interpreter;

  };

}

#endif
