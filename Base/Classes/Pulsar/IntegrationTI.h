//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/IntegrationTI.h

#ifndef __Pulsar_IntegrationTI_h
#define __Pulsar_IntegrationTI_h

#include "Pulsar/Integration.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Integration attributes
  class Integration::Interface : public TextInterface::To<Integration>
  {

  public:

    //! Default constructor that takes an optional instance
    Interface ( Integration* = 0 );

    //! Set the instance to which this interface interfaces
    void set_instance (Integration*) ;

    //! clone this text interface
    TextInterface::Parser *clone();

    //! Get the interface name
    std::string get_interface_name() const { return "Integration::Interface"; }

  };

}

#endif
