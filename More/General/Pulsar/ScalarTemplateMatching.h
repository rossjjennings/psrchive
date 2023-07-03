//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ScalarTemplateMatching.h

#ifndef __Pulsar_ScalarTemplateMatching_h
#define __Pulsar_ScalarTemplateMatching_h

#include "Pulsar/PhaseWeight.h"
#include "FTransformAgent.h"
#include "toa.h"

namespace Pulsar
{
  class Profile;
  class FluctuationSpectrumStats;

  //! The original scalar template matching algorithm
  /*! This class implements the Taylor (1992) algorithm, using the 
      Van Wijngaarden–Dekker–Brent method to minimize the objective merit function.
      
      The algorithm was implemented before psrchive existed, in a function named model_profile.
      There were many arguments to this function:
      
      int model_profile (unsigned npts, unsigned narrays,
                           const float* const* prf, const float* const* std,
                           double* scale, double* sigma_scale,
                           double* shift, double* sigma_shift,
                           double* chisq, bool verbose)
      
      and it was becoming a nuisance to maintain, extend or optimize it;
      therefore, this class was wrapped around it to manage complexity. */

  class ScalarTemplateMatching : public Reference::Able
  {
    //! Disable copy constructor
    ScalarTemplateMatching (const ScalarTemplateMatching& fit);

    //! Disable assignment operator
    ScalarTemplateMatching& operator = (const ScalarTemplateMatching& fit);

    //! Disable cloning
    ScalarTemplateMatching* clone () const;

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    ScalarTemplateMatching ();

    //! Destructor
    ~ScalarTemplateMatching ();

    //! Set the standard to which observations will be fit
    void set_standard (const Profile* standard);

    //! Get the standard to which observations will be fit
    const Profile* get_standard () const;

    //! Add the specified profile to be fit to the standard/template
    void add_observation ( const Profile* );

    //! Set the only profile to be fit to the standard/template
    void set_observation ( const Profile* );

    //! Delete any previously added observations
    void delete_observations ();

    //! Fit the specified observation to the standard
    void fit (const Profile* observation);

    //! Fit all observations to the standard
    void solve ();

    //! Get the phase offset between the standard and the observation in radians
    Estimate<double> get_phase () const;

    //! Get the scale factor between the standard and the observation
    Estimate<double> get_scale () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Get the maximum number of harmonics to include in fit
    unsigned get_maximum_harmonic () const { return maximum_harmonic; }

    //! Set the maximum number of harmonics to include in fit
    void set_choose_maximum_harmonic (bool flag);

    //! Get the maximum number of harmonics to include in fit
    bool get_choose_maximum_harmonic () const { return choose_maximum_harmonic; }

    //! Get the number of harmonics to be included in fit
    unsigned get_nharmonic () const { return n_harmonic; }

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the fourier transform plan
    void set_plan (FTransform::Plan*);

    //! When true, compute the reduced chisq; when false, assume it is unity
    void set_compute_reduced_chisq (bool flag) { compute_reduced_chisq = flag; }

    //! Get the statistical goodness of fit
    double get_reduced_chisq () const;

  protected:

    //! Used to compute the variance of the off-pulse harmonics
    Reference::To<FluctuationSpectrumStats> stats;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! Set true when set_standard should choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! The number of harmonics in the fit
    unsigned n_harmonic;

    //! Compute the reduced chisq (using the expected variance of the residual profile)
    /*! When set to false, incorrectly assume that the reduced chisq is equal to unity. */
    bool compute_reduced_chisq;

    //! Internal structure used to manage templates and observations
    class Data;

    //! Standards and observations
    std::vector<Data> data;

    //! Construtor helper
    void init ();

    //! Implement the solve method using the original model_profile algorithm
    void model_profile ();

    Estimate<double> best_shift;
    Estimate<double> best_scale;

    double chisq;
    unsigned nfree;

  private:

    //! True when the regions have been set
    bool regions_set;

  };

  class ScalarTemplateMatching::Data
  {
  public: 
    void set_stats (FluctuationSpectrumStats*);
    FluctuationSpectrumStats* get_stats ();

    void set_standard (const Profile*);
    const Profile* get_standard () const;
    const Profile* get_standard_fourier () const;

    void set_observation (const Profile*);
    const Profile* get_observation () const;
    const Profile* get_observation_fourier () const;

    //! Get the expected variance of the residual profile, given the best-fit scale factor
    double get_residual_variance (double scale) const;

  protected:

    //! Used to compute the variance of the off-pulse harmonics
    Reference::To<FluctuationSpectrumStats> stats;

    class ProfileData
    {
    public: 
      //! Default constructor
      ProfileData() { variance = 0.0; }

      //! A profile
      Reference::To<const Profile> profile;

      //! The Fourier transform of the profile
      Reference::To<const Profile> fourier;

      //! Estimate of the variance in the profile
      double variance;

    }; // class ProfileData

    ProfileData standard;
    ProfileData observation;

  }; // class ScalarTemplateMatching::Data

} // namespace Pulsar

#endif

