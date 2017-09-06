//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ExtensionInterpreter.h

#ifndef __Pulsar_ExtensionInterpreter_h
#define __Pulsar_ExtensionInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/Editor.h"

namespace Pulsar {

  class ExtensionInterpreter : public Interpreter::Extension {

    Pulsar::Editor editor;

  public:

    //! Default constructor
    ExtensionInterpreter ();

    //! Destructor
    ~ExtensionInterpreter ();

    //! Add new extension
    std::string add (const std::string& args);

    //! Remove extension
    std::string remove (const std::string& args);

    //! List extensions
    std::string list (const std::string& args);
  };

}

#endif
