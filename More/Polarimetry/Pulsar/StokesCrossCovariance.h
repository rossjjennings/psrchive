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

  //! Phase-resolved four-dimensional covariance matrix of Stokes parameters
  class StokesCrossCovariance : public Container {
    
  public:

    //! Construct from a CrossCovarianceMatrix object
    StokesCrossCovariance (const CrossCovarianceMatrix*);
    
    //! Clone operator
    virtual StokesCrossCovariance* clone () const;

    //! Get the number of bins
    unsigned get_nbin () const;

    //! Get the Stokes cross covariance for the specified pair of bins
    Matrix<4,4,double> get_cross_covariance (unsigned ibin,
					     unsigned jbin) const;

    //! Set the Stokes covariance for the specified bin
    void set_cross_covariance (unsigned ibin, unsigned jbin,
			       const Matrix<4,4,double>&);

  protected:

    unsigned nbin;

    unsigned get_icross (unsigned ibin, unsigned jbin) const;
    
    //! The cross covariance matrix for each phase bin pair
    std::vector< Matrix<4,4,double> > cross_covariance;
  };

}


#endif



