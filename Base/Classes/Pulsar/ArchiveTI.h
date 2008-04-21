//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.13 $
   $Date: 2008/04/21 01:58:31 $
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

    //! Default constructor that takes an optional instance
    ArchiveTI ( Archive* = 0 );

    //! Set the instance to which this interface interfaces
    void set_instance (Archive*) ;

    //! clone this text interface
    TextInterface::Parser *clone();
    
    //! Get the interface name
    std::string get_interface_name() { return "ArchiveTI"; }

  };

}


#endif
