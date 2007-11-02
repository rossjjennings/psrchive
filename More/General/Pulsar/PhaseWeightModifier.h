//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeightModifier.h,v $
   $Revision: 1.2 $
   $Date: 2007/11/02 04:25:39 $
   $Author: straten $ */

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

  protected:

    //! The PhaseWeight from which the PhaseWeight will be derived
    Reference::To<const PhaseWeight> input_weight;

    //! Derived classes implement the PhaseWeight calculation
    virtual void calculate (PhaseWeight*) = 0;

  };

}


#endif // !defined __Pulsar_PhaseWeightModifier_h
