//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/Home.h,v $
   $Revision: 1.1 $
   $Date: 2007/04/12 05:01:44 $
   $Author: straten $ */

#ifndef __Pulsar_Home_h
#define __Pulsar_Home_h

#include "Pulsar/Archive.h"
#include "Registry.h"

namespace Pulsar {

  /* Determines the installation directory. */
  class Home {

  public:
    
    // Return the name of the installation directory
    static std::string get_home ();
    
    // Return the name of the runtime directory
    static std::string get_runtime ();

  };

}

#endif
