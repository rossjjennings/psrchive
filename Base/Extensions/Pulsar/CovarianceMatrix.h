/***************************************************************************
 *
 * // CovarianceMatrix.h :: The Extension Class header for the COV_MAT PSRFITS Binary Table
 * 
 ***************************************************************************/

#ifndef __CovarianceMatrix_h
#define __CovarianceMatrix_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/Profile.h"
#include "Pulsar/TimeDomainCovariance.h" 

#include <vector> 

namespace Pulsar {

  class CovarianceMatrix : public Pulsar::Archive::Extension {

  public:
 
    //! Default constructor
    CovarianceMatrix ();

    //! Default Constructor [TDC]
    CovarianceMatrix(TimeDomainCovariance *t_cov);

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

    //! The vector to store the covariance matrix (made public for psrwhite)
      std::vector<double> covariance;   
         
    protected:

      unsigned nbin;      
            
      //! Pointer to TimeDomainCovariance Class to get the covariance matrix
      Pulsar::TimeDomainCovariance *tcov;

  };
     
}
#endif
