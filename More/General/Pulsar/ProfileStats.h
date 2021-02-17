//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileStats.h

#ifndef __Pulsar_ProfileStats_h
#define __Pulsar_ProfileStats_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Algorithm.h"
#include "Pulsar/HasBaselineEstimator.h"

#include "PhaseRange.h"
#include "TextInterface.h"
#include "Estimate.h"

namespace Pulsar
{
  class Profile;
  class ProfileWeightFunction;
  class SNRatioEstimator;
  class WidthEstimator;

  //! Computes pulse profile statistics
  class ProfileStats : public Algorithm, public HasBaselineEstimator
  {   
  public:

    //! Default constructor
    ProfileStats (const Profile* profile = 0);

    //! Copy constructor
    ProfileStats (const ProfileStats&);

    //! Destructor
    ~ProfileStats();

    //! Clone operator
    ProfileStats* clone () const;

    //! The algorithm used to find the included phase bins
    void set_include_estimator (ProfileWeightFunction*);
    ProfileWeightFunction* get_include_estimator () const;

    //! The algorithm used to find the excluded phase bins
    void set_exclude_estimator (ProfileWeightFunction*);
    ProfileWeightFunction* get_exclude_estimator () const;

    //! Set the Profile from which statistics will be derived
    void set_profile (const Profile*);
    void set_Profile (const Profile* profile) { set_profile(profile); }

    //! Set the Profile that defines the baseline and on-pulse regions
    void select_profile (const Profile*);

    //! Deselect onpulse phase bins in profile that fall below threshold
    void deselect_onpulse (const Profile* profile, float threshold);

    //! The algorithm used to find the on-pulse phase bins
    void set_onpulse_estimator (ProfileWeightFunction*);
    ProfileWeightFunction* get_onpulse_estimator () const;

    //! The algorithm used to find the off-pulse phase bins
    void set_baseline_estimator (ProfileWeightFunction*);

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the on-pulse and baseline regions
    void get_regions (PhaseWeight& pulse, PhaseWeight& base) const;

    //! Get the number of phase bins in the on pulse window
    unsigned get_onpulse_nbin () const;

    //! Return true if the specified phase bin is in the on pulse window
    bool get_onpulse (unsigned ibin) const;

    //! Set if the specified phase bin is in the on pulse window
    void set_onpulse (unsigned ibin, bool);

    //! Get the number of phase bins in the baseline window
    unsigned get_baseline_nbin () const;

    //! Return true if the specified phase bin is in the baseline window
    bool get_baseline (unsigned ibin) const;

    //! Returns the total flux of the on-pulse phase bins
    Estimate<double> get_total (bool subtract_baseline = true) const;

    //! Returns the variance of the baseline
    Estimate<double> get_baseline_variance () const;

    //! Return the on-pulse phase bin mask
    PhaseWeight* get_onpulse ();

    //! Return the off-pulse baseline mask
    PhaseWeight* get_baseline ();

    //! Return the all pulse phase bin mask
    PhaseWeight* get_all ();

    //! The algorithm used to estimate the signal-to-noise ratio
    void set_snratio_estimator (SNRatioEstimator*);
    SNRatioEstimator* get_snratio_estimator () const;

    //! Set the signal-to-noise ratio estimator
    void set_snr_estimator (const std::string& name);

    //! Get the signal-to-noise ratio
    double get_snr () const;

    //! Get the text interface of the signal-to-noise ratio estimator
    TextInterface::Parser* get_snr_interface ();

    //! The algorithm used to estimate the pulse width
    void set_width_estimator (WidthEstimator*);
    WidthEstimator* get_width_estimator () const;

    //! Set the pulse width estimator
    void set_pulse_width_estimator (const std::string& name);

    //! Get the pulse width
    ::Phase::Value get_pulse_width () const;

    //! Get the text interface of the pulse width estimator
    TextInterface::Parser* get_pulse_width_interface ();

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Text interface to methods
    class Interface;

  protected:

    //! The Profile from which statistics will be derived
    Reference::To<const Profile, false> profile;

    //! The algorithm used to find the on-pulse phase bins
    Reference::To<ProfileWeightFunction> onpulse_estimator;

    Reference::To<SNRatioEstimator> snratio_estimator;
    Reference::To<WidthEstimator> width_estimator;

    //! True when the onpulse and baseline regions have been set
    bool regions_set;

    //! The on-pulse phase bin mask
    mutable PhaseWeight onpulse;

    //! The off-pulse baseline mask
    mutable PhaseWeight baseline;

    //! All phase bins (subject to include and exclude)
    mutable PhaseWeight all;

    //! The variance of the total intensity baseline
    mutable Estimate<double> baseline_variance;

    //! The algorithm used to find the included phase bins
    mutable Reference::To<ProfileWeightFunction> include_estimator;

    //! The included phase bin mask
    mutable PhaseWeight include;

    //! The algorithm used to find the excluded phase bins
    mutable Reference::To<ProfileWeightFunction> exclude_estimator;

    //! The excluded phase bin mask
    mutable PhaseWeight exclude;

    //! Computes the phase bin masks
    void build () const;

  private:

    //! flag set when built
    mutable bool built;
  };

}


#endif



