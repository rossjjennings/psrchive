//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/GeneralizedChiSquared.h

#include "BinaryStatistic.h"

namespace BinaryStatistics
{
  //! Computes the generalized squared interpoint distance between vectors
  /*! If one of the vectors is the mean of the distribution, than this 
    distance is equivalent to the square of the Mahalanobis distance. */
  class GeneralizedChiSquared : public BinaryStatistic
  {
    bool robust_linear_fit;
    double max_zap_fraction;

    std::vector< std::vector<double> > eigenvectors;
    std::vector< double > inverse_eigenvalues;
    
  public:

    GeneralizedChiSquared ();

    void set_eigenvectors ( std::vector< std::vector<double> >& );
    void set_eigenvectors ( std::vector< double >& );
    void set_eigenvectors ( unsigned ndat, double* );

    void set_eigenvalues ( std::vector< double >& );
    void set_eigenvalues ( unsigned ndat, double* );

    double get (const std::vector<double>&, const std::vector<double>&);
    GeneralizedChiSquared* clone () const { return new GeneralizedChiSquared; }
  };
}
