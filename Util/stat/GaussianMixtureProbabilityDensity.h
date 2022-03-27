//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/GaussianMixtureProbabilityDensity.h

#include "BinaryStatistic.h"
#include "ndArray.h"

#include <armadillo>

namespace BinaryStatistics
{
  class GeneralizedChiSquared;
  
  //! Computes the Gaussian mixture probability density of the difference
  /*! Assumes that one of the vectors is the mean of the distribution. */
  class GaussianMixtureProbabilityDensity : public BinaryStatistic
  {

  public:

    //! Optionally compute the GM psd of the principal component residuals
    Reference::To<GeneralizedChiSquared> gcs;

    arma::gmm_diag* model;
    
    GaussianMixtureProbabilityDensity ();

    double get (const std::vector<double>&, const std::vector<double>&);

    //! Return the number of Gaussians
    unsigned get_ngaus () const;

    GaussianMixtureProbabilityDensity* clone () const
    { return new GaussianMixtureProbabilityDensity; }
    
  };
}
