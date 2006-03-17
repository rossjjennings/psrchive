//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/17 13:34:45 $
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
