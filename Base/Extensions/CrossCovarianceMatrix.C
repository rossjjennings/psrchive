/***************************************************************************
 *
 *   Copyright (C) 2016 by Aditya Parthasarathy & Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CrossCovarianceMatrix.h"
#include <stdlib.h> 

using namespace std;
using Pulsar::CrossCovarianceMatrix;

//! Default Constructor 
CrossCovarianceMatrix::CrossCovarianceMatrix () 
 : Extension ("CrossCovarianceMatrix")
{
  nbin = npol = 0;
  nlag = 1;
}

//! Copy constructor
CrossCovarianceMatrix::CrossCovarianceMatrix (const CrossCovarianceMatrix& ext)
 : Extension ("CrossCovarianceMatrix")
{
  operator = (ext);
}

//! Assignment operator
const CrossCovarianceMatrix&
CrossCovarianceMatrix::operator= (const CrossCovarianceMatrix& extension)
{
  nbin = extension.nbin;
  npol = extension.npol;
  nlag = extension.nlag;

  covariance = extension.covariance;

  return *this;
}

//! Destructor
CrossCovarianceMatrix::~CrossCovarianceMatrix ()
{
}

//! Sets the number of phase bins
void CrossCovarianceMatrix::set_nbin (unsigned _nbin)
{
  nbin = _nbin;
}

//! Returns the number of phase bins
unsigned CrossCovarianceMatrix::get_nbin () const
{
  return nbin;
}

//! Sets the number of phase bins
void CrossCovarianceMatrix::set_npol (unsigned _npol)
{
  npol = _npol;
}

//! Returns the number of phase bins
unsigned CrossCovarianceMatrix::get_npol () const
{
  return npol;
}

//! Sets the number of lags
void CrossCovarianceMatrix::set_nlag (unsigned _nlag)
{
  nlag = _nlag;
}

//! Returns the number of lags
unsigned CrossCovarianceMatrix::get_nlag () const
{
  return nlag;
}

void CrossCovarianceMatrix::resize ()
{
  unsigned M = npol * nbin;
  covariance.resize (nlag * M*(M+1)/2);  
}

