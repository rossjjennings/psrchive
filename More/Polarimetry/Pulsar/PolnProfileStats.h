//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileStats.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/13 07:25:19 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileStats_h
#define __Pulsar_PolnProfileStats_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Algorithm.h"
#include "Estimate.h"

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

    //! The algorithm used to find the on-pulse phase bins
    void set_on_pulse_estimator (OnPulseEstimator*);

    //! The algorithm used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total_intensity () const;

    //! Returns the total polarized flux of the on-pulse phase bins
    Estimate<double> get_total_polarized () const;

    //! Returns the total linearly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_linear () const;

    //! Returns the total circularly polarized flux of the on-pulse phase bins
    Estimate<double> get_total_circular () const;

    //! Returns the total absolute value of circularly polarized flux
    Estimate<double> get_total_abs_circular () const;

  protected:

    //! The PolnProfile from which statistics will be derived
    Reference::To<const PolnProfile> profile;

    //! The algorithm used to find the on-pulse phase bins
    Reference::To<OnPulseEstimator> on_pulse_estimator;

    //! The algorithm used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! The on-pulse phase bin mask
    PhaseWeight on_pulse;

    //! The off-pulse baseline mask
    PhaseWeight baseline;

    //! Computes the phase bin masks
    void build ();

    //! Sets up the algorithms to compute the required quantities
    void setup (const Profile*) const;

    //! Returns the total of the on-pulse window
    Estimate<double> get_total_on_pulse () const;

  };

}


#endif



