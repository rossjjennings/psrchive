//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.12 $
   $Date: 2007/10/02 05:19:26 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTI_h
#define __Pulsar_ArchiveTI_h

#include "Pulsar/Archive.h"
#include "TextInterface.h"

namespace Pulsar
{

  //! Provides a text interface to get and set Archive attributes
  class ArchiveTI : public TextInterface::To<Archive>
  {

  public:

    //! Constructor
    ArchiveTI ();

    //! Constructor that takes an instance
    ArchiveTI ( Archive *c );

    //! setup the get/set methods
    void setup( void );
    
    //! clone this ti
    TextInterface::Parser *clone();
    
    //! Get the interface name
    std::string get_interface_name() { return "ArchiveTI"; }

  };

}


#endif
