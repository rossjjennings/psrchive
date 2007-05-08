//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Attic/Generator.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/08 06:55:21 $
   $Author: straten $ */

#ifndef __PulsarGenerator_h
#define __PulsarGenerator_h

#include "Reference.h"

namespace Pulsar {

  class Predictor;

  //! Generator of functions that predict pulse phase
  /*! This pure virtual base class defines the interface to pulse
    phase predictor generators */
  class Generator : public Reference::Able {

  public:

    //! Return a new Predictor instance
    virtual Pulsar::Predictor* generate () const = 0;

  };

}

#endif
