//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileStats.h,v $
   $Revision: 1.5 $
   $Date: 2007/12/24 20:01:25 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileStats_h
#define __Pulsar_PolnProfileStats_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Algorithm.h"
#include "Estimate.h"
#include "Stokes.h"

namespace Pulsar {

  class PolnProfile;
  class BaselineEstimator;
  class OnPulseEstimator;

  //! Computes polarimetric pulse profile statistics
  class PolnProfileStats : public Algorithm {
    
  public:

    //! Default constructor
    PolnProfileStats (const PolnProfile* profile = 0);

    //! Destructor
    ~PolnProfileStats();

    //! Set the PolnProfile from which statistics will be derived
    void set_profile (const PolnProfile*);

    //! Set the PolnProfile that defines the baseline and on-pulse regions
    void select_profile (const PolnProfile*);

    //! The algorithm used to find the on-pulse phase bins
    void set_on_pulse_estimator (OnPulseEstimator*);

    //! The algorithm used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    unsigned get_on_pulse_nbin () const;
    unsigned get_baseline_nbin () const;

    //! Get the Stokes parameters for the specified phase bin
    Stokes< Estimate<double> > get_stokes (unsigned ibin) const;

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total_intensity () const;

    //! Returns the total polarized flux of the on-pulse phase bins
    Estimate<double> get_total_polarized () const;

    //! Returns the total determinant of the on-pulse phase bins
    Estimate<double> get_total_determinant () const;

    //! Returns the total linearly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_linear () const;

    //! Returns the total circularly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_circular () const;

    //! Returns the total absolute value of circularly polarized flux
    Estimate<double> get_total_abs_circular () const;

    //! Returns the variance of the baseline for the specified polarization
    Estimate<double> get_baseline_variance (unsigned ipol) const;

  protected:

    //! The PolnProfile from which statistics will be derived
    Reference::To<const PolnProfile> profile;

    //! The algorithm used to find the on-pulse phase bins
    Reference::To<OnPulseEstimator> on_pulse_estimator;

    //! The algorithm used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! When, true the on_pulse and baseline estimators have been selected
    bool estimators_selected;

    //! The on-pulse phase bin mask
    mutable PhaseWeight on_pulse;

    //! The off-pulse baseline mask
    mutable PhaseWeight baseline;

    //! The variance of the total intensity baseline
    mutable Stokes< Estimate<double> > baseline_variance;

    //! Computes the phase bin masks
    void build ();

    //! Sets up the algorithms to compute the required quantities
    void setup (const Profile*) const;

    //! Returns the total of the on-pulse window
    Estimate<double> get_total_on_pulse () const;

  };

}


#endif



