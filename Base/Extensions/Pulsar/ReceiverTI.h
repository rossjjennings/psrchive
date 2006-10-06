//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Pulsar_ReceiverTI_h
#define __Pulsar_ReceiverTI_h

#include "Pulsar/Receiver.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Receiver attributes
  class ReceiverTI : public TextInterface::To<Receiver> {

  public:

    //! Constructor
    ReceiverTI ();

  };

}

#endif
