//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Check.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/17 23:04:41 $
   $Author: straten $ */

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
    
  protected:

    // Checks registered for uses in Archive::load
    static Registry::List<Check> registry;
      
    // Declare friends with Registry::Entry<Check> so it can access registry
    friend class Registry::Entry<Check>;

    // Declare friends with Archive so Archive::load can access registry
    friend class Archive;


  };

}

#endif
