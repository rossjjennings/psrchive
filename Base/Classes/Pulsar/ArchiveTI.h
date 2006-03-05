//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/05 15:11:44 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTI_h
#define __Pulsar_ArchiveTI_h

#include "Pulsar/Archive.h"
#include "Pulsar/ExtensionTI.h"

namespace Pulsar {

  //! Provides a text interface to get and set Archive attributes
  class ArchiveTI : public TextInterface::CompositeGetSet<Archive> {

  public:

    //! Constructor
    ArchiveTI () { init(); }

    //! Import the TextInterface of an Extension class
    template<class Ext>
    void import (const std::string& name,
		 TextInterface::ClassGetSet<Ext>* extension_tui);

    //! Import the TextInterface of the Integration class
    void import (const std::string& name,
                 TextInterface::ClassGetSet<Integration>* integration_tui);

  private:

    //! Constructor work
    void init ();

  };

}

template<class Ext>
void Pulsar::ArchiveTI::import (const std::string& name,
				 TextInterface::ClassGetSet<Ext>* tui)
{
  TextInterface::CompositeGetSet<Archive>::import 
    ( new ExtensionTI<Archive,Ext>(name, tui) );
}


#endif
