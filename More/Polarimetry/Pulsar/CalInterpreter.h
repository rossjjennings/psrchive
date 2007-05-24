//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/24 10:54:35 $
   $Author: straten $ */

#ifndef __Pulsar_CalInterpreter_h
#define __Pulsar_CalInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Database;
  class PolnCalibrator;
  
  class CalInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    CalInterpreter ();

    //! Destructor
    ~CalInterpreter ();

    //! cal command interface
    std::string cal (const std::string& args);

    //! load the specifed file (database or calibrator)
    std::string load (const std::string& arg);

    //! calibrate the current top of the stack using the current state
    std::string calibrate ();

  protected:

    //! The type of calibrator to be used
    Calibrator::Type type;

    //! Calibrator database
    Reference::To<Database> database;

    //! Calibrator to be applied
    Reference::To<PolnCalibrator> calibrator;

  };

}

#endif
