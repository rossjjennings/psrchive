//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/FluxCentroid.h

#ifndef __Pulsar_FluxCentroid_h
#define __Pulsar_FluxCentroid_h

#include "Pulsar/ProfileShiftEstimator.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Estimates phase shift using the flux "centre of mass"
  class FluxCentroid : public ProfileShiftEstimator
  {

  public:

    //! Default constructor
    FluxCentroid ();

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Set the duty cycle over which flux will be integrated
    void set_duty_cycle (float dc) { duty_cycle = dc; }
    float get_duty_cycle () const { return duty_cycle; }

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<FluxCentroid> ("COF"); }

    //! Return a copy constructed instance of self
    FluxCentroid* clone () const { return new FluxCentroid(*this); }

  protected:

    //! Duty cycle of pulse, over which flux will be integrated
    float duty_cycle;
  };

}


#endif // !defined __Pulsar_FluxCentroid_h
