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

    //! Get a copy of the covariance matrix
    void get_covariance_matrix_copy ( double* dest );
      
    //! Get the row, col covariance matrix element
    double get_covariance_matrix_value ( unsigned row, unsigned col );

    //! This normalizes the covariance matrix after all profiles were added
    void finalize ();

    //! Compute the eigen decomposition
    void eigen ();

    //! Set the eigenvector matrix
    void set_eigenvectors ( const std::vector<double>& );
    void set_eigenvectors ( const double* );

    //! Get a copy of the eigenvector matrix
    void get_eigenvectors_copy ( std::vector<double>& );
    void get_eigenvectors_copy ( double* );
    
    double get_eigenvectors_value ( unsigned row, unsigned col );
    const double* get_eigenvectors_pointer ();

    //! Set the eigenvalue vector
    void set_eigenvalues ( const std::vector<double>& );
    void set_eigenvalues ( const double* );

    //! Get a copy of the eigenvalue vector
    void get_eigenvalues_copy ( std::vector<double>& );
    void get_eigenvalues_copy ( double* );
    
    double get_eigenvalue ( unsigned row );
    const double* get_eigenvalues_pointer ();
    
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
