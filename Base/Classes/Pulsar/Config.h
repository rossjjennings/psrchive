//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/01 22:18:33 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

#include "Configuration.h"

namespace Pulsar {

  //! Stores PSRCHIVE configuration parameters
  class Config : public Configuration {

  public:

    //! Default constructor loads $PSRCHIVE/psrchive.cfg
    Config ();

  };

  //! Global configuration
  extern Config config;

}

#endif
