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
    float error;
    float baseline_duty_cycle;
    float fraction_of_maximum;
    
  public:

    //! Default constructor
    PhaseWidth ();
    
    //! Return the signal to noise ratio based on the shift
    Phase::Value get_width (const Profile* profile);

    float get_error () const { return error; }
    float get_baseline_duty_cycle () const { return baseline_duty_cycle; }
    void set_baseline_duty_cycle (float);
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

