/***************************************************************************
 *
 *   Copyright (C) 2016 - 2021 by Willem van Straten
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
StokesCrossCovariance::StokesCrossCovariance
(const CrossCovarianceMatrix* matrix)
{
  if (!matrix)
    return;

  nbin = matrix->get_nbin();
  nlag = matrix->get_nlag();
  
  unsigned npol = matrix->get_npol();

  if (npol != 4)
    throw Error (InvalidState,
		 "StokesCrossCrossCovariancePlot::get_profiles",
		 "CovarianceMatrix::npol=%u != 4", npol);

  const vector<double>& data = matrix->get_data();
  unsigned idat=0;

  cross_covariance.resize( get_ncross_total() );
  unsigned icov=0;
  
  for (unsigned ilag = 0; ilag < nlag ; ilag ++)
  {
    for (unsigned ibin = 0; ibin < nbin ; ibin ++)
    {
      unsigned startbin = (ilag == 0) ? ibin : 0;
      
      for (unsigned jbin = startbin; jbin < nbin ; jbin ++)
      {
	Matrix<4,4,double>& c = cross_covariance[icov];
	icov ++;
      
	for (unsigned ipol=0; ipol < npol; ipol++)
        {
	  // on the diagonal, take only the upper triangle
	  unsigned startpol = (ilag == 0 && ibin == jbin) ? ipol : 0;

	  for (unsigned jpol = startpol; jpol < npol ; jpol++)
	  {
	    c[ipol][jpol] = data.at(idat);
	    idat ++;

	    if (ilag == 0 && ibin == jbin)
	      c[jpol][ipol] = c[ipol][jpol];
	  }
	}
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

void StokesCrossCovariance::set_nbin (unsigned _nbin)
{
  nbin = _nbin;
}

//
//
//
unsigned StokesCrossCovariance::get_nlag () const
{
  return nlag;
}

void StokesCrossCovariance::set_nlag (unsigned _nlag)
{
  nlag = _nlag;
}

//
//
//
unsigned StokesCrossCovariance::get_icross (unsigned ibin,
					    unsigned jbin,
					    unsigned ilag) const
{
  unsigned icross = 0;
  
  if (ilag == 0)
  {
    if (jbin < ibin)
      std::swap (ibin, jbin);

    // icross = 0 + nbin + nbin-1 + nbin-2 + nbin-3,
    //          where the number of terms = ibin+1
    //          offset by jbin, which starts at ibin
    // Although ibin-1 overflows when ibin==0, it is multiplied by 0
    icross = ibin * nbin - ((ibin * (ibin-1)) / 2) + (jbin - ibin);
  }
  else
  {
    icross = get_ncross (0) + (ilag - 1) * get_ncross(1) + ibin * nbin + jbin;
  }

  if (icross >= get_ncross_total())
    throw Error (InvalidRange,
		 "Pulsar::StokesCrossCovariance::get_icross",
		 "nbin=%u ibin=%u jbin=%u ilag=%u -> icross=%u >= ncross=%u",
		 nbin, ibin, jbin, ilag, icross, get_ncross_total());

  return icross;
}

unsigned StokesCrossCovariance::get_ncross (unsigned ilag) const
{
  if (ilag == 0)
    return (nbin * (nbin+1)) / 2;
  else
    return nbin * nbin;
}

unsigned StokesCrossCovariance::get_ncross_total () const
{
  return get_ncross(0) + (nlag - 1) * get_ncross(1);
}

//
//
//
Matrix<4,4,double> 
StokesCrossCovariance::get_cross_covariance (unsigned ibin,
					     unsigned jbin,
					     unsigned ilag) const
{
  check (ibin, jbin, ilag, "StokesCrossCovariance::get_cross_covariance");
  return cross_covariance[ get_icross(ibin,jbin,ilag) ];
}

//
//
//
void StokesCrossCovariance::set_cross_covariance (unsigned ibin,
						  unsigned jbin,
						  const Matrix<4,4,double>& C)
{
  set_cross_covariance (ibin, jbin, 0, C);
}

//
//
//
void StokesCrossCovariance::set_cross_covariance (unsigned ibin,
						  unsigned jbin,
						  unsigned ilag,
						  const Matrix<4,4,double>& C)
{
  check (ibin, jbin, ilag, "StokesCrossCovariance::set_cross_covariance");
  cross_covariance[ get_icross(ibin,jbin) ] = C;
}

void StokesCrossCovariance::check (unsigned ibin,
				   unsigned jbin,
				   unsigned ilag,
				   const char* method) const
{
  if (ibin >= get_nbin())
    throw Error (InvalidRange, method,
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  if (jbin >= get_nbin())
    throw Error (InvalidRange, method,
		 "jbin=%d >= nbin=%d", ibin, get_nbin());

  if (ilag >= get_nlag())
    throw Error (InvalidRange, method,
		 "ilag=%d >= nlag=%d", ilag, get_nlag());
}


void StokesCrossCovariance::resize ()
{
  cross_covariance.resize (get_ncross_total());
  
  // verify self-consistency of index calculations
  assert ( get_icross (nbin-1, nbin-1, 0) == get_ncross(0)-1 );
}
