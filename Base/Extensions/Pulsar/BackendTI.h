//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/BackendTI.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/07 16:02:33 $
   $Author: straten $ */

#ifndef __Pulsar_BackendTI_h
#define __Pulsar_BackendTI_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Backend attributes
  class BackendTI : public TextInterface::To<Backend> {

  public:

    //! Constructor
    BackendTI ();

  };

}

#endif
