//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/GeneralizedChiSquared.h

#include "BinaryStatistic.h"
#include "ndArray.h"

namespace BinaryStatistics
{
  //! Computes the generalized squared interpoint distance between vectors
  /*! If one of the vectors is the mean of the distribution, than this 
    distance is equivalent to the square of the Mahalanobis distance. */
  class GeneralizedChiSquared : public BinaryStatistic
  {
    bool robust_linear_fit;
    double outlier_threshold;
    double max_zap_fraction;

  public:

    GeneralizedChiSquared ();

    double get (const std::vector<double>&, const std::vector<double>&);

    GeneralizedChiSquared* clone () const { return new GeneralizedChiSquared; }

    ndArray<2,double> eigenvectors;
    ndArray<1,double> eigenvalues;
    
  };
}
