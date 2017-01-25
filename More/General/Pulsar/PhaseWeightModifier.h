//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWeightModifier.h

#ifndef __Pulsar_PhaseWeightModifier_h
#define __Pulsar_PhaseWeightModifier_h

#include "Pulsar/PhaseWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! PhaseWeight algorithms that receive an input PhaseWeight
  class PhaseWeightModifier : public PhaseWeightFunction {

  public:

    //! Set the PhaseWeight from which the PhaseWeight will be derived
    virtual void set_weight (const PhaseWeight*);

    //! Returns a PhaseWeight 
    virtual void get_weight (PhaseWeight* weight);

    //! Inplace operator
    void operator () (PhaseWeight*);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

  protected:

    //! The PhaseWeight from which the PhaseWeight will be derived
    Reference::To<const PhaseWeight> input_weight;

    //! Derived classes implement the PhaseWeight calculation
    virtual void calculate (PhaseWeight*) = 0;

  };

}


#endif // !defined __Pulsar_PhaseWeightModifier_h
