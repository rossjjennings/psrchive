//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.2 $
   $Date: 2004/12/16 21:08:13 $
   $Author: straten $ */

#ifndef __Pulsar_ReceiverTUI_h
#define __Pulsar_ReceiverTUI_h

#include "Pulsar/Receiver.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class ReceiverTUI : public TextInterface::ClassGetSet<Receiver> {

  public:

    //! Constructor
    ReceiverTUI ();

  private:

    //! Constructor work
    void init ();

  };

}

#endif
