//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileStats.h,v $
   $Revision: 1.8 $
   $Date: 2008/06/19 06:52:28 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileStats_h
#define __Pulsar_ProfileStats_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Algorithm.h"
#include "Estimate.h"

namespace Pulsar
{
  class Profile;
  class BaselineEstimator;
  class OnPulseEstimator;

  //! Computes pulse profile statistics
  class ProfileStats : public Algorithm
  {   
  public:

    //! Default constructor
    ProfileStats (const Profile* profile = 0);

    //! Destructor
    ~ProfileStats();

    //! Set the Profile from which statistics will be derived
    void set_profile (const Profile*);

    //! Set the Profile that defines the baseline and on-pulse regions
    void select_profile (const Profile*);

    //! Deselect onpulse phase bins in profile that fall below threshold
    void deselect_onpulse (const Profile* profile, float threshold);

    //! The algorithm used to find the on-pulse phase bins
    void set_on_pulse_estimator (OnPulseEstimator*);

    //! The algorithm used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& pulse, PhaseWeight& base) const;

    //! Get the number of phase bins in the on pulse window
    unsigned get_on_pulse_nbin () const;

    //! Return true if the specified phase bin is in the on pulse window
    bool get_on_pulse (unsigned ibin) const;

    //! Set if the specified phase bin is in the on pulse window
    void set_on_pulse (unsigned ibin, bool);

    //! Get the number of phase bins in the baseline window
    unsigned get_baseline_nbin () const;

    //! Return true if the specified phase bin is in the baseline window
    bool get_baseline (unsigned ibin) const;

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total (bool subtract_baseline = true) const;

    //! Returns the variance of the baseline
    Estimate<double> get_baseline_variance () const;

    //! Return the on-pulse phase bin mask
    const PhaseWeight* get_on_pulse ();

    //! Return the off-pulse baseline mask
    const PhaseWeight* get_baseline ();

  protected:

    //! The Profile from which statistics will be derived
    Reference::To<const Profile> profile;

    //! The algorithm used to find the on-pulse phase bins
    Reference::To<OnPulseEstimator> on_pulse_estimator;

    //! The algorithm used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

    //! True when the on_pulse and baseline regions have been set
    bool regions_set;

    //! The on-pulse phase bin mask
    mutable PhaseWeight on_pulse;

    //! The off-pulse baseline mask
    mutable PhaseWeight baseline;

    //! The variance of the total intensity baseline
    mutable Estimate<double> baseline_variance;

    //! Computes the phase bin masks
    void build ();

    //! Sets up the algorithms to compute the required quantities
    void setup (const Profile*) const;

  };

}


#endif



