//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SimplePolnProfile.h

#ifndef __Pulsar_SimplePolnProfile_h
#define __Pulsar_SimplePolnProfile_h

#include "Pulsar/SyntheticPolnProfile.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/Gaussian.h"

namespace Pulsar {

  //! Simulates a simple Gaussian with a standard RVM and zero circular
  class SimplePolnProfile {

  public:

    //! Default constructor
    SimplePolnProfile ();

    //! Destructor
    ~SimplePolnProfile ();

    //! The synthetic polarimetric profile generator
    SyntheticPolnProfile generate;

    //! The gaussian total intensity profile
    MEAL::Gaussian gaussian;

    //! The rotating vector model used to describe the position angle
    MEAL::RotatingVectorModel rvm;

    //! Number of bins in pulse profile
    unsigned nbin;

    //! Relative noise
    float noise;

    //! Phase offset of simulated observations
    float phase;

  };

}

#endif
