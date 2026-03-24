//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/HasMaxHarmonic.h

#ifndef __Pulsar_HasMaxHarmonic_h
#define __Pulsar_HasMaxHarmonic_h

namespace Pulsar {

  //! Can be inherited by a ShiftEstimator that has a maximum harmonic
  class HasMaxHarmonic
  {
  public:

    //! Set the maximum number of harmonics to include in fit
    virtual void set_maximum_harmonic (unsigned max) { maximum_harmonic = max; }

    //! Get the maximum number of harmonics to include in fit
    virtual unsigned get_maximum_harmonic () const { return maximum_harmonic; }

    //! Automatically choose the maximum number of harmonics to include in fit
    virtual void set_choose_maximum_harmonic (bool flag) { choose_maximum_harmonic = flag; }

    //! Automatically choose the maximum number of harmonics to include in fit
    bool get_choose_maximum_harmonic () const { return choose_maximum_harmonic; }

  protected:

    //! The maximum number of harmonics to include in fit
    unsigned maximum_harmonic = 0;

    //! Set true when algorithm should choose the maximum harmonic
    bool choose_maximum_harmonic = false;
  };

}

#endif // !defined __Pulsar_HasMaxHarmonic_h
