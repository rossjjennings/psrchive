//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/07 16:02:33 $
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
