//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CovarianceMatrix.h

#ifndef __CovarianceMatrix_h
#define __CovarianceMatrix_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Profile.h"

#include <vector> 

namespace Pulsar {

  class CovarianceMatrix : public Pulsar::Archive::Extension
  {

  public:
 
    //! Default constructor
    CovarianceMatrix ();

    //! Copy constructor
    CovarianceMatrix (const CovarianceMatrix& extension);  

    //! Assignment operator
    const CovarianceMatrix& operator= (const CovarianceMatrix& extension);

    //!Destructor
    ~CovarianceMatrix ();

    //! Clone method
    CovarianceMatrix* clone () const { return new CovarianceMatrix( *this ); }

    //! Set the number of phase bins
    void set_nbin(unsigned nbin);
    
    //! Get the number of phase bins
    unsigned get_nbin() const;

    //! Set the number of polarizations
    void set_npol(unsigned npol);
    
    //! Get the number of polarizations
    unsigned get_npol() const;

    //! Set the size of the data array according to nbin and npol
    void resize ();
    
    //! Get the covariance matrix data
    std::vector<double>& get_data () { return covariance; }
    
    //! Get the covariance matrix data
    const std::vector<double>& get_data () const { return covariance; }
    
    protected:

    unsigned nbin;
    unsigned npol;
    
    std::vector<double> covariance;
            
  };
     
}

#endif

