//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationTI.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/07 16:02:33 $
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
