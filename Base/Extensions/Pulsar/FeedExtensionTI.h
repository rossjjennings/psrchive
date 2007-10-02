//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/FeedExtensionTI.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 04:50:09 $
   $Author: straten $
*/


#ifndef __Pulsar_FeedExtensionTI_h
#define __Pulsar_FeedExtensionTI_h

#include "Pulsar/FeedExtension.h"
#include <TextInterface.h>


namespace Pulsar
{

  //! Provides a text interface to get and set FeedExtension attributes
  class FeedExtensionTI : public TextInterface::To<FeedExtension>
  {

  public:

    //! Constructor
    FeedExtensionTI ();
    FeedExtensionTI ( FeedExtension *c );
    void SetupMethods( void );

    virtual std::string get_interface_name() { return "FeedExtensionTI"; }

    TextInterface::Parser *clone();
  };

}

#endif
