//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.1 $
   $Date: 2004/12/19 16:04:37 $
   $Author: straten $ */

#ifndef __Pulsar_PointingTUI_h
#define __Pulsar_PointingTUI_h

#include "Pulsar/Pointing.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class PointingTUI : public TextInterface::ClassGetSet<Pointing> {

  public:

    //! Constructor
    PointingTUI ();

  private:

    //! Constructor work
    void init ();

  };

}

#endif
