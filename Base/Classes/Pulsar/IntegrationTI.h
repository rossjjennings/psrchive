//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationTI.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/17 13:34:51 $
   $Author: straten $ */

#ifndef __Pulsar_IntegrationTI_h
#define __Pulsar_IntegrationTI_h

#include "Pulsar/Integration.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Integration attributes
  class IntegrationTI : public TextInterface::To<Integration> {

  public:

    //! Constructor
    IntegrationTI ();

  };

}

#endif
