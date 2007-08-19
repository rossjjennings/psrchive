//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.5 $
   $Date: 2007/08/19 19:55:44 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

#include "Configuration.h"

namespace Pulsar {

  //! Stores PSRCHIVE configuration parameters
  class Config {

  public:

    // Return the name of the installation directory
    static std::string get_home ();
    
    // Return the name of the runtime directory
    static std::string get_runtime ();

    //! Get the value for the specified key
    template<typename T> 
    static T get (const std::string& key, T default_value)
    {
      if (!config)
	load_config ();

      return config->get (key, default_value);
    }

  protected:

    //! The global configuration file
    static Configuration* config;
    
    //! Load the global configuration file
    static void load_config ();

  };

}

#endif
