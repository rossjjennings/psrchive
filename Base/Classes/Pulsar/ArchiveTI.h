//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Attic/ArchiveTI.h,v $
   $Revision: 1.8 $
   $Date: 2006/03/07 16:02:33 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveTI_h
#define __Pulsar_ArchiveTI_h

#include "Pulsar/Archive.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Archive attributes
  class ArchiveTI : public TextInterface::To<Archive> {

  public:

    //! Constructor
    ArchiveTI ();

  };

}


#endif
