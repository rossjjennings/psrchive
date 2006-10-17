//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Verification.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/17 14:55:11 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveVerification_h
#define __Pulsar_ArchiveVerification_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  /*! Verification classes define sanity checks that are performed
    before an Archive instance is written to disk. */
  class Archive::Verification : public Reference::Able {

  public:
    
    //! Check the Archive and throw an exception on error
    void check (Archive*) const throw Error = 0;

  };

}
