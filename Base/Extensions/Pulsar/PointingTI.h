//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/PointingTI.h,v $
   $Revision: 1.7 $
   $Date: 2007/06/20 03:04:53 $
   $Author: nopeer $ */

#ifndef __Pulsar_PointingTI_h
#define __Pulsar_PointingTI_h

#include "Pulsar/Pointing.h"
#include "TextInterface.h"

namespace Pulsar
{

  //! Provides a text interface to get and set Pointing attributes
  class PointingTI : public TextInterface::To<Pointing>
  {

  public:

    //! Constructor
    PointingTI ();
    PointingTI ( Pointing *c );
    void SetupMethods( void );
    
    virtual std::string get_interface_name() { return "PointingTI"; }

    TextInterface::Class *clone();
  };

}

#endif
