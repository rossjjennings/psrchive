/***************************************************************************
 *
 * // CovarianceMatrix.C :: The Extension Class for the COV_MAT PSRFITS Binary Table
 * 
 ***************************************************************************/

#include "Pulsar/CovarianceMatrix.h"
#include <stdlib.h> 

using namespace std;
using Pulsar::CovarianceMatrix;

//! Default Constructor 
CovarianceMatrix::CovarianceMatrix () : Extension ("CovarianceMatrix")
{
   nbin =0;
}

//! Default Constructor for TDC Class
CovarianceMatrix::CovarianceMatrix(TimeDomainCovariance *t_cov) : Extension ("CovarianceMatrix")
{
  tcov = t_cov;
  nbin = 0;
}

//! Copy constructor
CovarianceMatrix::CovarianceMatrix (const CovarianceMatrix& extension) : Extension ("CovarianceMatrix")
{
  operator = (extension);
}

//! Assignment operator
const CovarianceMatrix&
CovarianceMatrix::operator= (const CovarianceMatrix& extension)
{
  
  nbin = extension.nbin;
  covariance = extension.covariance;

  return *this;
}

//! Destructor
CovarianceMatrix::~CovarianceMatrix ()
{
}

//! Sets the number of phase bins
void CovarianceMatrix::set_nbin(unsigned nbin)
{
  covariance.resize(nbin*nbin);  
}

//! Returns the number of phase bins from TDC Class
unsigned CovarianceMatrix::get_nbin() const // Write a method in TDC!
{
   //return tcov->get_nbins();
}

//! Obtains the Covariance Matrix as produced by TDC Class
void CovarianceMatrix::get_matrix(double *dest) const
{
  tcov->get_covariance_matrix( dest );    
}

//! Sets the values in the Covariance Matrix to the vector 'covariance'
void CovarianceMatrix::set_matrix(double *dest)
{  
  for(int i =0; i<covariance.size(); i++)  
      covariance.at(i) = dest[i];        
}






