//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationTI.h,v $
   $Revision: 1.1 $
   $Date: 2004/12/20 13:29:30 $
   $Author: straten $ */

#ifndef __Pulsar_IntegrationTI_h
#define __Pulsar_IntegrationTI_h

#include "Pulsar/Integration.h"
#include "Pulsar/ExtensionTI.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
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
