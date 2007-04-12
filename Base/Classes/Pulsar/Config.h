//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.4 $
   $Date: 2007/04/12 05:08:07 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

#include "Configuration.h"

namespace Pulsar {

  //! Stores PSRCHIVE configuration parameters
  class Config : public Configuration {

  public:

    //! Default constructor loads psrchive.cfg from the installation directory
    Config ();

    // Return the name of the installation directory
    static std::string get_home ();
    
    // Return the name of the runtime directory
    static std::string get_runtime ();

  };

  //! Global configuration
  extern Config config;

}

#endif
