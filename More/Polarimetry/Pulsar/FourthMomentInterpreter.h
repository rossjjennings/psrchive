//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FourthMomentInterpreter.h

#ifndef __Pulsar_FourthMomentInterpreter_h
#define __Pulsar_FourthMomentInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class FourthMomentInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    FourthMomentInterpreter ();

    //! Destructor
    ~FourthMomentInterpreter ();

    //! remove the bias due to noise
    std::string debias (const std::string& arg);

  };

}

#endif
