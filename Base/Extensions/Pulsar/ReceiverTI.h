//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/ReceiverTI.h,v $
   $Revision: 1.1 $
   $Date: 2004/12/16 16:59:20 $
   $Author: straten $ */

#ifndef __Pulsar_ReceiverTUI_h
#define __Pulsar_ReceiverTUI_h

#include "TextInterface.h"

namespace Pulsar {

  class Receiver;

  //! Smooths a pulse profile using the mean over a boxcar
  class ReceiverTUI : public TextInterface::ClassGetSet<Receiver> {

  public:

    //! Constructor
    ReceiverTUI () { init(); }

  private:

    //! Constructor work
    void init ();

  };

}

#endif
