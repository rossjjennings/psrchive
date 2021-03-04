//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CrossCovarianceMatrix.h

#ifndef __CrossCovarianceMatrix_h
#define __CrossCovarianceMatrix_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Profile.h"

#include <vector> 

namespace Pulsar {

  class CrossCovarianceMatrix : public Pulsar::Archive::Extension
  {

  public:
 
    //! Default constructor
    CrossCovarianceMatrix ();

    //! Copy constructor
    CrossCovarianceMatrix (const CrossCovarianceMatrix& extension);  

    //! Assignment operator
    const CrossCovarianceMatrix& operator= (const CrossCovarianceMatrix&);

    //!Destructor
    ~CrossCovarianceMatrix ();

    //! Clone method
    CrossCovarianceMatrix* clone () const 
    { return new CrossCovarianceMatrix( *this ); }

    //! Set the number of phase bins
    void set_nbin (unsigned);
    
    //! Get the number of phase bins
    unsigned get_nbin () const;

    //! Set the number of polarizations
    void set_npol (unsigned);
    
    //! Get the number of polarizations
    unsigned get_npol () const;

    //! Set the number of lags
    void set_nlag (unsigned);

    //! Get the number of lags
    unsigned get_nlag () const;

    //! Set the size of the data array according to nbin and npol
    void resize ();
    
    //! Get the covariance matrix data
    std::vector<double>& get_data () { return covariance; }
    
    //! Get the covariance matrix data
    const std::vector<double>& get_data () const { return covariance; }
    
    protected:

    unsigned nbin;
    unsigned npol;
    unsigned nlag;

    std::vector<double> covariance;
            
  };
     
}

#endif

