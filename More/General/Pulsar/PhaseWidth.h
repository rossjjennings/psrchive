//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWidth.h

#ifndef __Pulsar_PhaseWidth_h
#define __Pulsar_PhaseWidth_h

#include "Pulsar/WidthEstimator.h"
#include "Pulsar/HasBaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"

namespace Pulsar {

  //! Calculates the pulse width in the phase domain
  /*! This default implementation of the width estimator is taken from pdv. */
  class PhaseWidth : public WidthEstimator, public HasBaselineEstimator
  {
    // stores the estimated uncertainty in pulse width estimate
    Phase::Value error;

    // height at which to estimate width
    float fraction_of_maximum;

    // threshold above noise below which algorithm gives up
    float threshold_above_noise;

    //! The off-pulse baseline mask
    mutable PhaseWeight baseline;
    
  public:

    //! Default constructor
    PhaseWidth ();
    
    //! Return the width of the pulse profile in turns
    double get_width_turns (const Profile* profile);

    Phase::Value get_error () const { return error; }

    float get_fraction_of_maximum () const { return fraction_of_maximum; }
    void set_fraction_of_maximum (float);
    
    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    PhaseWidth* clone () const;

  };

}

#endif

