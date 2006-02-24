//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/BackendTI.h,v $
   $Revision: 1.1 $
   $Date: 2006/02/24 22:50:30 $
   $Author: straten $ */

#ifndef __Pulsar_BackendTI_h
#define __Pulsar_BackendTI_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Provides a text interface to get and set Backend attributes
  class BackendTI : public TextInterface::ClassGetSet<Backend> {

  public:

    //! Constructor
    BackendTI ();

  private:

    //! Constructor work
    void init ();

  };

}

#endif
