/***************************************************************************
 *  *
 *  *   Copyright (C) 2011 by Stefan Oslowski
 *  *   Licensed under the Academic Free License version 2.1
 *  *
 ****************************************************************************/

#ifndef __Pulsar__TimeDomainCovariance_h
#define __Pulsar__TimeDomainCovariance_h

#include "Pulsar/ProfileCovariance.h"

namespace Pulsar
{
  class TimeDomainCovariance : public ProfileCovariance
  {
  public:
    
    //! Default constructor
    TimeDomainCovariance ();

    //! Choose which bins are to be used in the calculation of covariance matrix
    void choose_bins ( unsigned, unsigned );

    //! Get the name of class used
    std::string get_name () const;

    //! Adds the given Profile to the covariance matrix estimate
    void add_Profile (const Profile* );
    void add_Profile (const Profile*, float wt );

    //! Set the whole covariance matrix
    void set_covariance_matrix ( const double * );

    //! Get the whole covariance matrix
    void get_covariance_matrix_copy ( double* dest );
      
    //! Get the row, col covariance matrix element
    double get_covariance_matrix_value ( unsigned row, unsigned col );

    //! This normalizes the covariance matrix after all profiles were added
    void finalize ();

    //! Compute the eigen decomposition
    void eigen ();

    //! Get the eigenvectors
    void get_eigenvectors_copy ( double* dest );
    double get_eigenvectors_value ( unsigned row, unsigned col );

    //! Get the count of profiles used to estimate the covariance matrix
    unsigned get_count ();

    //! Set the count of profiles used to estimate the covariance matrix
    void set_count ( unsigned );

    //! Get the rank of covariance matrix
    unsigned get_rank ();

    //! Set the rank of covariance matrix
    void set_rank ( unsigned );

    //! Reset the covariance matrix calculations
    void reset ();

  protected:
    
    //! first bin to be used
    unsigned first_bin;

    //! last bin to be used
    unsigned last_bin;

    bool subtract_mean;

    //! Current covariance matrix (symmetric)
    std::vector<double> covariance_matrix;
    
    //! Current mean profile
    std::vector<double> mean;

    //! Eigenvectors (row vectors in row-major order)
    std::vector<double> eigenvectors;
    
    //! Eigenvalues
    std::vector<double> eigenvalues;

    bool eigen_decomposed;

  };
}

#endif
