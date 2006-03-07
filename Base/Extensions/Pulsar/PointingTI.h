//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/07 16:02:33 $
   $Author: straten $ */

#ifndef __Pulsar_PointingTI_h
#define __Pulsar_PointingTI_h

#include "Pulsar/Pointing.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Pointing attributes
  class PointingTI : public TextInterface::To<Pointing> {

  public:

    //! Constructor
    PointingTI ();

  };

}

#endif
