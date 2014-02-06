//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FixInterpreter.h

#ifndef __Pulsar_FixInterpreter_h
#define __Pulsar_FixInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/FixFluxCal.h"
#include "Pulsar/SetReceiver.h"

namespace Pulsar
{

  class FixInterpreter : public Interpreter::Extension 
  {

  public:

    //! Default constructor
    FixInterpreter ();

    //! Destructor
    ~FixInterpreter ();

    //! fix the Archive::Type (FluxCalOn or Off)
    std::string fluxcal (const std::string& args);

    //! fix the receiver information
    std::string receiver (const std::string& args);

    //! offset the epoch of each sub-integration 
    std::string epoch (const std::string& args);

    //! recompute the frequency of each sub-band
    std::string freq (const std::string& args);

    //! fix the Pointing extension info
    std::string pointing (const std::string& args);

    //! no empty commands
    std::string empty ();

  protected:

    FixFluxCal fix_flux_cal;
    SetReceiver set_receiver;

  };

}

#endif
