//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.3 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Pulsar_PointingTI_h
#define __Pulsar_PointingTI_h

#include "Pulsar/Pointing.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Pointing attributes
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
