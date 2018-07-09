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
#include <vector>

namespace Pulsar {

  //! Calculates the pulse width in the phase domain
  /*! This default implementation of the width estimator is taken from pdv. */
  class PhaseWidth : public WidthEstimator
  {
    Phase::Value error;
    Phase::Value baseline_duty_cycle;
    float fraction_of_maximum;
    
  public:

    //! Default constructor
    PhaseWidth ();
    
    //! Return the width of the pulse profile in turns
    double get_width_turns (const Profile* profile);

    Phase::Value get_error () const { return error; }

    //! Set the duty cycle used to search for the off-pulse baseline
    void set_baseline_duty_cycle (const Phase::Value& w)
    { baseline_duty_cycle = w; }

    //! Get the baseline_duty_cycle of the smoothing function
    Phase::Value get_baseline_duty_cycle () const
    { return baseline_duty_cycle; }

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

