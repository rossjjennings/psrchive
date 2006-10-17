//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Correction.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/17 14:54:47 $
   $Author: straten $ */

#ifndef __Pulsar_Correction_h
#define __Pulsar_Correction_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  /*! Correction classes define corrections to be performed after an
    Archive instance is loaded from disk. */
  class Archive::Correction : public Reference::Able {

  public:
    
    //! Apply the correction to the Archive
    void apply (Archive*) const = 0;

  };

}
