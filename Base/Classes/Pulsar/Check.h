//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/Check.h

#ifndef __Pulsar_Check_h
#define __Pulsar_Check_h

#include "Pulsar/Archive.h"
#include "Registry.h"

namespace Pulsar {

  /* This abstract base class organizes various correction and verification 
     operations. */
  class Archive::Check : public Reference::Able {

  public:
    
    // Return the name of the check
    virtual std::string get_name () = 0;

    // Disable the named check
    static void disable (const std::string& name);

    // Stupid hack to make sure the Check registry gets
    // linked in.
    static void ensure_linkage ();

  protected:

  };

}

#endif
