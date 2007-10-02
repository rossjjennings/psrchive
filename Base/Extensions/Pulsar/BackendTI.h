//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/BackendTI.h,v $
   $Revision: 1.6 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $ */

#ifndef __Pulsar_BackendTI_h
#define __Pulsar_BackendTI_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

namespace Pulsar
{

  //! Provides a text interface to get and set Backend attributes
  class BackendTI : public TextInterface::To<Backend>
  {

  public:

    //! Constructor
    BackendTI ();
    BackendTI ( Backend *c );
    void SetupMethods( void );

    virtual std::string get_interface_name() { return "BackendTI"; }

    TextInterface::Parser *clone();
  };

}

#endif
