/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCrossCovariance.h"
#include "Pulsar/CrossCovarianceMatrix.h"

#include <assert.h>

using namespace std;
using namespace Pulsar;

//
//
//
StokesCrossCovariance::StokesCrossCovariance (const CrossCovarianceMatrix* matrix)
{
  if (!matrix)
    return;

  nbin = matrix->get_nbin();
  unsigned npol = matrix->get_npol();

  if (npol != 4)
    throw Error (InvalidState,
		 "StokesCrossCrossCovariancePlot::get_profiles",
		 "CovarianceMatrix::npol=%u != 4", npol);

  const vector<double>& data = matrix->get_data();
  unsigned idat=0;

  cross_covariance.resize( nbin * (nbin+1) / 2 );
  unsigned icov=0;
  
  for (unsigned ibin = 0; ibin < nbin ; ibin ++)
  {
    for (unsigned jbin = ibin; jbin < nbin ; jbin ++)
    {
      Matrix<4,4,double>& c = cross_covariance[icov];
      icov ++;
      
      for (unsigned ipol=0; ipol < npol; ipol++)
	for (unsigned jpol = (ibin == jbin) ? ipol : 0; jpol < npol ; jpol++)
	{
	  c[ipol][jpol] = data.at(idat);
	  idat ++;

	  if (ibin == jbin)
	    c[jpol][ipol] = c[ipol][jpol];
	}
    }
  }

  assert (icov == cross_covariance.size());
  assert (idat == data.size());
}

//
//
//
StokesCrossCovariance* StokesCrossCovariance::clone () const
{
  return new StokesCrossCovariance (*this);
}

//
//
//
unsigned StokesCrossCovariance::get_nbin () const
{
  return nbin;
}

//
//
//
unsigned StokesCrossCovariance::get_icross (unsigned ibin, unsigned jbin) const
{
  if (jbin < ibin)
    std::swap (ibin, jbin);
      
  // icross = 0 + nbin + nbin-1 + nbin-2 + nbin-3,
  //          where the number of terms = ibin+1
  //          offset by jbin, which starts at ibin
  // Although ibin-1 overflows when ibin==0, it is multiplied by 0
  unsigned icross = ibin * nbin - (ibin * (ibin-1) / 2) + (jbin - ibin);

  if (icross >= cross_covariance.size())
    throw Error (InvalidRange,
		 "Pulsar::StokesCrossCovariance::get_icross",
		 "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		 nbin, ibin, jbin, icross, cross_covariance.size());

  return icross;
}

//
//
//
Matrix<4,4,double> 
StokesCrossCovariance::get_cross_covariance (unsigned ibin,
						     unsigned jbin) const
{
  if (ibin >= get_nbin())
    throw Error (InvalidRange, "StokesCrossCovariance::get_covariance",
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  if (jbin >= get_nbin())
    throw Error (InvalidRange, "StokesCrossCovariance::get_covariance",
		 "jbin=%d >= nbin=%d", ibin, get_nbin());

  return cross_covariance[ get_icross(ibin,jbin) ];
}

//
//
//
void StokesCrossCovariance::set_cross_covariance (unsigned ibin,
						  unsigned jbin,
						  const Matrix<4,4,double>& C)
{
 if (ibin >= get_nbin())
    throw Error (InvalidRange, "StokesCrossCovariance::get_covariance",
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  if (jbin >= get_nbin())
    throw Error (InvalidRange, "StokesCrossCovariance::get_covariance",
		 "jbin=%d >= nbin=%d", ibin, get_nbin());

  cross_covariance[ get_icross(ibin,jbin) ] = C;
}
