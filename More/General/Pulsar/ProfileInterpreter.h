//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/09/24 08:04:55 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileInterpreter_h
#define __Pulsar_ProfileInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class ProfileInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    ProfileInterpreter ();

    //! Destructor
    ~ProfileInterpreter ();

    //! Form the cummulative profile
    std::string cumulative (const std::string& args);

    //! Form the difference profile
    std::string difference (const std::string& args);

    //! Form the mean smoothed profile
    std::string mean (const std::string& args);

    //! Form the median smoothed profile
    std::string median (const std::string& args);

    //! No empty arguments
    std::string empty ();

  };

}

#endif
