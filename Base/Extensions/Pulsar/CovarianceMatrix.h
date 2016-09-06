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

    //! Set the covariance matrix
    void set_matrix(double *dest);
    
    //! Get the covariance matrix
    void get_matrix(double *dest) const;   

    protected:

    unsigned nbin;      
    std::vector<double> covariance;
            
  };
     
}

#endif

