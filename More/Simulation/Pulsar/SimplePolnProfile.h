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
  class SimplePolnProfile : public SyntheticPolnProfile
  {

  public:

    //! Default constructor
    SimplePolnProfile ();

    //! Destructor
    ~SimplePolnProfile ();

    //! Get the Rotating Vector Model used to simulate linear polarization
    MEAL::RotatingVectorModel* get_RVM () { return &rvm; }

    //! Centre the phase centre of the profile
    void set_centre (double);

  protected:

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
