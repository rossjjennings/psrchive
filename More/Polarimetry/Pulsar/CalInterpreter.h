//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CalInterpreter.h

#ifndef __Pulsar_CalInterpreter_h
#define __Pulsar_CalInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/Calibrator.h"

#include "Pulsar/FscrunchInterpreter.h"
#include "Pulsar/PolnCalExtFreqIntegrate.h"

#include "MEAL/Complex2.h"

namespace Pulsar {

  class Database;
  class PolnCalibrator;
  class HybridCalibrator;
  class FluxCalibrator;

  class CalInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    CalInterpreter ();

    //! Destructor
    ~CalInterpreter ();

    //! perform the backend convention corrections
    std::string backend (const std::string& arg);

    // calibrate the current top of the stack using the current state
    std::string cal (const std::string& args);

    // flux calibrate the current top of the stack using the current state
    std::string fluxcal (const std::string& args);

    //! cal command interface
    std::string type (const std::string& args);

    //! load the specifed file (database or calibrator)
    std::string load (const std::string& arg);

    //! set the calibrator match criteria
    std::string match (const std::string& arg);

    //! perform the frontend (basis and projection) correction
    std::string frontend (const std::string& arg);

    //! normalize profiles by absolute gain
    std::string set_gain (const std::string& arg);

    //! frequency integrate the loaded PolnCalibrator
    std::string fscrunch (const std::string& arg);

  protected:

    //! The type of calibrator to be used
    Reference::To<const Calibrator::Type> caltype;

    //! Calibrator database
    Reference::To<Database> database;

    //! Calibrator to be applied
    Reference::To<PolnCalibrator> calibrator;

    //! Hybrid calibrator to be applied
    Reference::To<HybridCalibrator> hybrid_calibrator;

    //! Flux calibrator to be applied
    Reference::To<FluxCalibrator> flux_calibrator;

    //! The transformation to be applied
    Reference::To<MEAL::Complex2> transformation;

    //! Manages fscrunch interpretation
    FscrunchInterpreter<PolnCalExtFreqIntegrate> fscruncher;

  };

}

#endif
