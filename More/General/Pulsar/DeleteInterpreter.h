//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten and Fabian Jankowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/DeleteInterpreter.h

#ifndef __Pulsar_DeleteInterpreter_h
#define __Pulsar_DeleteInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class DeleteInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    DeleteInterpreter ();

    //! Destructor
    ~DeleteInterpreter ();

    //! delete the specified channels
    std::string chan (const std::string& args);

    //! delete the specified integrations
    std::string subint (const std::string& args);

    //! delete the specified ranges of frequencies
    std::string freq (const std::string& args);

    //! delete the specified channels from the CalibratorExtension
    std::string cal (const std::string& args);

    //! no empty commands
    std::string empty ();

  };

}

#endif
