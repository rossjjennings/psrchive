//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationTI.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_IntegrationTI_h
#define __Pulsar_IntegrationTI_h

#include "Pulsar/Integration.h"
#include "Pulsar/ExtensionTI.h"

namespace Pulsar {

  //! Provides a text interface to get and set Integration attributes
  class IntegrationTI : public TextInterface::CompositeGetSet<Integration> {

  public:

    //! Constructor
    IntegrationTI ();

    //! Import the TextInterface of an Extension class
    template<class Ext>
    void import (const std::string& name,
                 TextInterface::ClassGetSet<Ext>* extension_tui);

  private:

    //! Constructor work
    void init ();

  };

}

template<class Ext>
void Pulsar::IntegrationTI::import (const std::string& name,
                                    TextInterface::ClassGetSet<Ext>* tui)
{
  TextInterface::CompositeGetSet<Integration>::import
    ( new ExtensionTI<Integration,Ext>(name, tui) );
}

#endif
