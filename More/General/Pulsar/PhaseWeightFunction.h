//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWeightFunction.h

#ifndef __Pulsar_PhaseWeightFunction_h
#define __Pulsar_PhaseWeightFunction_h

#include "Pulsar/Algorithm.h"
#include "TextInterface.h"

namespace Pulsar {

  class PhaseWeight;

  //! Algorithms that produce PhaseWeight objects
  /*! This pure virtual base class of PhaseWeight algorithms defines the
      interface by which various routines, such as baseline estimators
      can be accessed */
  class PhaseWeightFunction : public Algorithm
  {

  public:

    //! Retrieve the PhaseWeight
    virtual void get_weight (PhaseWeight* weight) = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Return a copy constructed instance of self
    virtual PhaseWeightFunction* clone () const = 0;
  };

}


#endif // !defined __Pulsar_PhaseWeightFunction_h
