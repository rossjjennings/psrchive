//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.3 $
   $Date: 2004/12/19 18:53:09 $
   $Author: straten $ */

#ifndef __Pulsar_ReceiverTI_h
#define __Pulsar_ReceiverTI_h

#include "Pulsar/Receiver.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
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
