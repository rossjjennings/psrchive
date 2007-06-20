//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FITSHdrExtensionTI.h,v $
   $Revision: 1.2 $
   $Date: 2007/06/20 03:04:53 $
   $Author: nopeer $ */


#include <TextInterface.h>
#include "FITSHdrExtension.h"



namespace Pulsar
{
  class FITSHdrExtensionTI : public TextInterface::To< FITSHdrExtension >
  {
  public:
    FITSHdrExtensionTI();
    FITSHdrExtensionTI( FITSHdrExtension *c );
    void SetupMethods( void );
    
    virtual std::string get_interface_name() { return "FITSHdrExtensionTI"; }

    TextInterface::Class *clone(); 
  };
}


