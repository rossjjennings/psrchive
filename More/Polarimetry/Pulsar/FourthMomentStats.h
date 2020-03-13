//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FourthMomentStats.h

#ifndef __Pulsar_FourthMomentStats_h
#define __Pulsar_FourthMomentStats_h

#include "Pulsar/PolnProfileStats.h"
#include "Matrix.h"

namespace Pulsar {

  class Archive;
  class StokesCovariance;

  //! Computes fourth-order moment pulse profile statistics
  class FourthMomentStats : public PolnProfileStats {
    
  public:

    //! Default constructor
    FourthMomentStats (const PolnProfile* profile = 0);

    //! Destructor
    ~FourthMomentStats();

    //! Worker function corrects bias in output of psr4th
    static void debias (Archive*, bool xcovar = true);
    
    void set_profile (const PolnProfile* _profile);
    void set_bandwidth (double bw);
    void set_duration (double T) { duration = T; }
    void set_folding_period (double P) { folding_period = P; }

    //! Get the estimated covariance matrix for the specified phase bin
    Matrix< 4,4,Estimate<double> > get_covariance (unsigned ibin) const;

    const StokesCovariance* get_covariance () const;

    //! Get the eigen polarization profiles
    void eigen (PolnProfile* = 0, PolnProfile* = 0, PolnProfile* = 0);

    void smooth_eigenvectors (PolnProfile*, PolnProfile*, PolnProfile*);

    //! Get the eigen values of the polarization vector space
    const Profile* get_eigen_value (unsigned k);

    //! Get the regression coefficients for each polarization vector
    const Profile* get_regression_coefficient (unsigned k);

    //! Get covariance between polarized and total intensity in natural basis
    const Profile* get_natural_covariance (unsigned k);

    //! Get the mode-separated profiles
    void separate (PolnProfile& modeA, PolnProfile& modeB);

    //! Return the phase-resolved modulation index
    Reference::To<Profile> get_modulation_index();

    Reference::To<Profile> get_nonorthogonality() { return nonorthogonality; }

    Reference::To<Profile> get_sin_theta() { return sin_theta; }
    Reference::To<Profile> get_cos_theta() { return cos_theta; }
    Reference::To<Profile> get_norm_theta() { return norm_theta; }

    //! Returns the variance of the baseline for the specified moment
    Estimate<double> get_moment_variance (unsigned) const;

  protected:

    Reference::To<const StokesCovariance> covariance;

    //! The integration length
    double duration;

    //! The folding period
    double folding_period;

    //! The bandwidth in MHz
    double bandwidth;

    std::vector< Reference::To<Profile> > eigen_value;
    std::vector< Reference::To<Profile> > regression_coefficient;
    std::vector< Reference::To<Profile> > natural_covariance;
    Reference::To<Profile> nonorthogonality;

    Reference::To<Profile> sin_theta;
    Reference::To<Profile> cos_theta;
    Reference::To<Profile> norm_theta;

    //! The variance of the baseline for each moment
    mutable std::vector< Estimate<double> > moment_variance;

    void smooth (unsigned poln, unsigned ibin, Stokes<double>& match,
                 std::vector<PolnProfile*>& eigen_vector);

  };

}


#endif



