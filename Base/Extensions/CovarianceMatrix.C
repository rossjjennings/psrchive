/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CovarianceMatrix.h"
#include <stdlib.h> 

using namespace std;
using Pulsar::CovarianceMatrix;

//! Default Constructor 
CovarianceMatrix::CovarianceMatrix () : Extension ("CovarianceMatrix")
{
  nbin = npol = 0;
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
  npol = extension.npol;
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
}

//! Returns the number of phase bins
unsigned CovarianceMatrix::get_nbin() const
{
  return nbin;
}

//! Sets the number of phase bins
void CovarianceMatrix::set_npol(unsigned _npol)
{
  npol = _npol;
}

//! Returns the number of phase bins
unsigned CovarianceMatrix::get_npol() const
{
  return npol;
}

void CovarianceMatrix::resize ()
{
  unsigned M = npol * nbin;
  covariance.resize (M*(M+1)/2);  
}
