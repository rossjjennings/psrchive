//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.3 $
   $Date: 2004/12/16 21:09:45 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTUI_h
#define __Pulsar_ArchiveTUI_h

#include "Pulsar/Archive.h"
#include "Pulsar/ExtensionTUI.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class ArchiveTUI : public TextInterface::CompositeGetSet<Archive> {

  public:

    //! Constructor
    ArchiveTUI () { init(); }

    //! Process a command
    void process (const std::string& command);

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
void Pulsar::ArchiveTUI::import (const std::string& name,
				 TextInterface::ClassGetSet<Ext>* tui)
{
  TextInterface::CompositeGetSet<Archive>::import 
    ( new ExtensionTUI<Archive,Ext>(name, tui) );
}


#endif
