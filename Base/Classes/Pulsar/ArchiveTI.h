//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.4 $
   $Date: 2004/12/19 18:53:09 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTI_h
#define __Pulsar_ArchiveTI_h

#include "Pulsar/Archive.h"
#include "Pulsar/ExtensionTI.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class ArchiveTI : public TextInterface::CompositeGetSet<Archive> {

  public:

    //! Constructor
    ArchiveTI () { init(); }

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
void Pulsar::ArchiveTI::import (const std::string& name,
				 TextInterface::ClassGetSet<Ext>* tui)
{
  TextInterface::CompositeGetSet<Archive>::import 
    ( new ExtensionTI<Archive,Ext>(name, tui) );
}


#endif
