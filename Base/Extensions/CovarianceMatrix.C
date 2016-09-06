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
  nbin = 0;
}

//! Copy constructor
CovarianceMatrix::CovarianceMatrix (const CovarianceMatrix& extension)
 : Extension ("CovarianceMatrix")
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
void CovarianceMatrix::set_nbin(unsigned _nbin)
{
  nbin = _nbin;
  covariance.resize (nbin*(nbin+1)/2);  
}

//! Returns the number of phase bins from TDC Class
unsigned CovarianceMatrix::get_nbin() const // Write a method in TDC!
{
  return nbin;
}

