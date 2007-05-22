//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/Parameters.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/22 22:27:38 $
   $Author: straten $ */

#ifndef __PulsarParameters_h
#define __PulsarParameters_h

#include "Reference.h"

#include <stdio.h>

namespace Pulsar {

  //! Storage of pulsar parameters used to create a Predictor
  /*! This pure virtual base class defines the interface to pulsar
    parameters */
  class Parameters : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Return a new, copy constructed instance of self
    virtual Parameters* clone () const = 0;

    //! Return true if *this == *that
    virtual bool equals (const Parameters* that) = 0;

    //! Load from an open stream
    virtual void load (FILE*) = 0;

    //! Unload to an open stream
    virtual void unload (FILE*) const = 0;

    //! Factory helper creates a vector of pointers to derived class instances
    static void children (std::vector< Reference::To<Parameters> >&);

  };

}

#endif
