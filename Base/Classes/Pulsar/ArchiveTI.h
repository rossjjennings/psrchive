//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.1 $
   $Date: 2004/12/16 11:42:20 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTUI_h
#define __Pulsar_ArchiveTUI_h

#include "TextInterface.h"

namespace Pulsar {

  class Archive;

  //! Smooths a pulse profile using the mean over a boxcar
  class ArchiveTUI : public TextInterface::ClassGetSet<Archive> {

  public:

    //! Constructor
    ArchiveTUI () { init(); }

    //! Process a command
    void process (const std::string& command);

  private:

    //! Constructor work
    void init ();

  };

}

#endif
