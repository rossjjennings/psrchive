//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.2 $
   $Date: 2004/12/19 18:53:09 $
   $Author: straten $ */

#ifndef __Pulsar_PointingTI_h
#define __Pulsar_PointingTI_h

#include "Pulsar/Pointing.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class PointingTI : public TextInterface::ClassGetSet<Pointing> {

  public:

    //! Constructor
    PointingTI ();

  private:

    //! Constructor work
    void init ();

  };

}

#endif
