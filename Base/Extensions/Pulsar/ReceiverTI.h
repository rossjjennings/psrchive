//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.4 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Pulsar_ReceiverTI_h
#define __Pulsar_ReceiverTI_h

#include "Pulsar/Receiver.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Receiver attributes
  class ReceiverTI : public TextInterface::ClassGetSet<Receiver> {

  public:

    //! Constructor
    ReceiverTI ();

  private:

    //! Constructor work
    void init ();

  };

}

#endif
