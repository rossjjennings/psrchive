//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/StokesCrossCovariance.h

#ifndef __Pulsar_StokesCrossCovariance_h
#define __Pulsar_StokesCrossCovariance_h

#include "Pulsar/Container.h"

#include "Stokes.h"
#include "Jones.h"
#include "Matrix.h"

namespace Pulsar {

  class CrossCovarianceMatrix;

  //! Cross-covariances between the Stokes parameters 
  /*! Computed as a function of lag in turns for all pulse longitude pairs */
  class StokesCrossCovariance : public Container
  {
    
  public:

    //! Construct from a CrossCovarianceMatrix object
    StokesCrossCovariance (const CrossCovarianceMatrix* = 0);

    //! Load from a CrossCovarianceMatrix object
    void load (const CrossCovarianceMatrix*);

    //! Unload to a CrossCovarianceMatrix object
    void unload (CrossCovarianceMatrix*);

    class Stream;
    
    //! Clone operator
    virtual StokesCrossCovariance* clone () const;

    //! Get the number of longitude bins
    unsigned get_nbin () const;
    void set_nbin (unsigned);

    //! Get the number of lage
    unsigned get_nlag () const;
    void set_nlag (unsigned);

    //! Pepare to store
    void resize ();

    //! Set all values to specified value
    void set_all (double val = 0.0);
 
    //! Get the Stokes cross covariance for the specified pair of bins
    Matrix<4,4,double> get_cross_covariance (unsigned ibin,
					     unsigned jbin,
					     unsigned ilag = 0) const;

    //! Get the Stokes cross covariance for the specified pair of bins
    Matrix<4,4,double>& get_cross_covariance (unsigned ibin,
                                              unsigned jbin,
                                              unsigned ilag = 0);

    //! Set the Stokes covariance for the specified bin
    void set_cross_covariance (unsigned ibin, unsigned jbin,
			       const Matrix<4,4,double>&, bool lock = false);

    //! Set the Stokes covariance for the specified bin
    void set_cross_covariance (unsigned ibin, unsigned jbin, unsigned ilag,
			       const Matrix<4,4,double>&, bool lock = false);

    //! Get the offset for the specified bin pair and lag
    unsigned get_icross (unsigned ibin, unsigned jbin, unsigned ilag=0) const;

    //! Get the number of cross-covariance matrices for the specified lag
    unsigned get_ncross (unsigned ilag) const;

    //! Get the total number of cross-covariance matrices
    unsigned get_ncross_total () const;

  protected:

    unsigned nbin;
    unsigned nlag;

    unsigned get_icross_check (unsigned ibin, unsigned jbin, unsigned ilag,
			       const char*) const;
    
    //! The cross covariance matrix for each phase bin pair
    std::vector< Matrix<4,4,double> > cross_covariance;
    std::vector< bool > locked;
  };

}


#endif



