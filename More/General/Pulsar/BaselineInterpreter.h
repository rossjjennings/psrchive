//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/07/30 06:02:31 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineInterpreter_h
#define __Pulsar_BaselineInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class BaselineInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    BaselineInterpreter ();

    //! Destructor
    ~BaselineInterpreter ();

    //! Install the interative Gaussian baseline algorithm
    std::string gaussian (const std::string& args);

    //! Install the default baseline algorithm (minimum window)
    std::string minimum (const std::string& args);

    //! No empty arguments
    std::string empty ();

  };

}

#endif
