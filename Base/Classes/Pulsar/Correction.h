//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/Correction.h

#ifndef __Pulsar_Correction_h
#define __Pulsar_Correction_h

#include "Pulsar/Check.h"

namespace Pulsar {

  /*! Pure abstract base class of corrections to be performed after an
    Archive instance is loaded from disk. */
  class Correction : public Archive::Check {

  public:
    
    //! Apply the correction to the Archive
    virtual void apply (Archive*) = 0;

  };

}

#endif
