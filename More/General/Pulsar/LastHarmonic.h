//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/LastHarmonic.h

#ifndef __Pulsar_LastHarmonic_h
#define __Pulsar_LastHarmonic_h

#include "Pulsar/RiseFall.h"
#include "Pulsar/LastSignificant.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Finds the last significant harmonic in a flucuation power spectral density
  /*!
    The last significant harmonic is defined by number of consecutive points above threshold.

    This class violates strong behavioural subtyping (aka Liskov Substitution Principle) by 
    inheriting RiseFall, which inherits OnPulseEstimator, which inherits ProfileWeightFunction. 
    It does so only to re-use the machinery of these classes.  The Profile object passed to
    this object is also not a phase-resolved average; it is the squared modulus of the Fouier
    transform of what we usually call a Profile.
  */

  class LastHarmonic : public RiseFall
  {

  public:

    //! Default constructor
    LastHarmonic ();

    //! Set the flucuation power spectral density from which the last harmonic will be computed
    /*! The profile passed to this function should be the flucuation power spectral density;
        i.e. the squared modulus of the Fourier transform of an average pulse profile */
    void set_Profile (const Profile* psd);

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Set the number of consecutive points that must remain above threshold
    void set_consecutive (unsigned consecutive);

    //! Get the number of consecutive points that must remain above threshold
    unsigned get_consecutive () const;

    //! Get the index of the last significant harmonic
    unsigned get_last_harmonic () const;

    //! Set the BaselineEstimator used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Get the BaselineEstimator used to find the off-pulse phase bins
    const BaselineEstimator* get_baseline_estimator () const;
    BaselineEstimator* get_baseline_estimator ();

    const PhaseWeight* get_baseline () const;

    //! Get the harmonic indeces of the rise and fall (rise always equals 1)
    void get_indeces (int& rise, int& fall) const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return new copy of self
    LastHarmonic* clone () const;

  protected:

    //! Compute the rise and fall of the pulse
    void build ();

    //! Flag set when the rise and fall are computed
    bool built;

    int bin_rise;
    int bin_fall;

    //! The estimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! The basline returned by the estimator
    Reference::To<PhaseWeight> baseline;

    //! The algorithm used to find the last signficant harmonic
    LastSignificant significant;

    Reference::To<const Profile> fluctuation_power_spectral_density;
  };

}

#endif // !defined __Pulsar_LastHarmonic_h
