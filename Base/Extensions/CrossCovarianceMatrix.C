/***************************************************************************
 *
 *   Copyright (C) 2016 - 2021 by Aditya Parthasarathy & Willem van Straten
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

unsigned CrossCovarianceMatrix::get_ncross (unsigned ilag) const
{
  unsigned M = npol * nbin;
  if (ilag == 0)
    return (M * (M+1)) / 2;
  else
    return M * M;
}

unsigned CrossCovarianceMatrix::get_ncross_total () const
{
  return get_ncross(0) + (nlag - 1) * get_ncross(1);
}

void CrossCovarianceMatrix::resize_data ()
{
  covariance.resize ( get_ncross_total() );
}

//! Return true if covariances are stored in a stream of blocks
bool CrossCovarianceMatrix::has_stream () const
{
  return stream;
}

//! Set the stream of covariances stored in blocks
void CrossCovarianceMatrix::set_stream (Stream* s)
{
  stream = s;
}

//! Get the stream of covariances stored in blocks
const CrossCovarianceMatrix::Stream* 
CrossCovarianceMatrix::get_stream () const
{
  return stream;
}


