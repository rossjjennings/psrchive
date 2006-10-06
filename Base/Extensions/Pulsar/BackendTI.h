//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/BackendTI.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:05:50 $
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
