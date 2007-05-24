//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/InterpreterExtension.h,v $
   $Revision: 1.2 $
   $Date: 2007/05/24 08:42:30 $
   $Author: straten $ */

#ifndef __Pulsar_Interpreter_Extension_h
#define __Pulsar_Interpreter_Extension_h

#include "Pulsar/Interpreter.h"

namespace Pulsar {

  class Interpreter::Extension : public CommandParser {

  public:

    enum Status { Good, Warn, Fail };

    //! get the current Pulsar
    Archive* get ()
      { return interpreter->get(); }

    std::vector<std::string> setup (const std::string& text)
      { return interpreter->setup (text); }

    std::string response (Status s, const std::string& text = "")
      { return interpreter->response ((Interpreter::Status)s, text); }

  protected:

    friend class Interpreter;

    //! The parent Interpreter
    Reference::To<Interpreter> interpreter;

  };

}

#endif
