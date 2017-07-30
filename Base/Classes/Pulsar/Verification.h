//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/Verification.h

#ifndef __Pulsar_ArchiveVerification_h
#define __Pulsar_ArchiveVerification_h

#include "Pulsar/Check.h"

namespace Pulsar {

  /*! Pure abstract base class of sanity checks performed before an
    Archive instance is written to disk. */
  class Verification : public Archive::Check {

  public:
    
    //! Check the Archive and throw an exception on error
    virtual void apply (const Archive*) = 0;

  };

}

#endif
