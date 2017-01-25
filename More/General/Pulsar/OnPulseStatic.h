//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/OnPulseStatic.h

#ifndef __Pulsar_OnPulseStatic_h
#define __Pulsar_OnPulseStatic_h

#include "Pulsar/OnPulseEstimator.h"

namespace Pulsar
{
  class OnPulseStatic : public OnPulseEstimator
  {

  public:

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return new copy of self
    OnPulseStatic* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);
  };
}

#endif // !defined __Pulsar_OnPulseStatic_h
