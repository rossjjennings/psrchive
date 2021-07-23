
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
StokesCrossCovariance::StokesCrossCovariance (const CrossCovarianceMatrix* mat)
{
  nbin = nlag = 0;
  
  if (mat)
    load (mat);
}

void StokesCrossCovariance::load (const CrossCovarianceMatrix* matrix)
{
  nbin = matrix->get_nbin();
  nlag = matrix->get_nlag();
  
  unsigned npol = matrix->get_npol();

  if (npol != 4)
    throw Error (InvalidState, "StokesCrossCovariance::load",
		 "CovarianceMatrix::npol=%u != 4", npol);

#if _DEBUG
  cerr << "StokesCrossCovariance::load"
    " nbin=" << matrix->get_nbin() <<
    " npol=" << matrix->get_npol() <<
    " nlag=" << matrix->get_nlag() <<
    " ndat=" << matrix->get_data().size() << endl;
#endif
  
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

//! Pure virtual base class of a stream of covariances stored in blocks
class StokesCrossCovariance::Stream : public CrossCovarianceMatrix::Stream
{
  StokesCrossCovariance* parent;
  CrossCovarianceMatrix* to;
  mutable unsigned ilag;
  mutable unsigned ibin;
  mutable unsigned jbin;
  mutable unsigned ipol;
  mutable unsigned jpol;
  mutable unsigned current_offset;

  unsigned nlag;
  unsigned nbin;
  unsigned npol;
  unsigned ncross_total;

 public:

  Stream (StokesCrossCovariance* _parent, CrossCovarianceMatrix* _to) 
  {
    parent = _parent;
    to = _to;
    ilag = ibin = jbin = ipol = jpol = current_offset = 0;
    nlag = parent->get_nlag();
    nbin = parent->get_nbin();
    npol = 4;
    ncross_total = to->get_ncross_total();
  }

  ~Stream ()
  {
#if _DEBUG
    cerr << "StokesCrossCovariance::Stream dtor" << endl;
#endif
    verify_end_of_data ();
  }

  void verify_end_of_data () const
  {
#if _DEBUG
    cerr << "StokesCrossCovariance::Stream::verify_end_of_data \n"
	 << " off=" << current_offset << " ncross=" << ncross_total
	 << " ilag=" << ilag << " ibin=" << ibin << " jbin=" << jbin
	 << " ipol=" << ipol << " jpol=" << jpol << endl;
#endif

    assert (current_offset == ncross_total);

    assert (ilag+1 == nlag);
    assert (ibin+1 == nbin);
    assert (jbin+1 == nbin);
    assert (ipol+1 == npol);
    assert (jpol == npol);
  }

  unsigned get_ndat () const { return to->get_ncross_total(); }

  void get_data (unsigned off, unsigned n, double* dat) const;
};

void StokesCrossCovariance::unload (CrossCovarianceMatrix* matrix)
{
  unsigned npol = 4;
  
  matrix->set_nbin (nbin);
  matrix->set_npol (npol);
  matrix->set_nlag (nlag);
  matrix->set_stream ( new Stream(this, matrix) );

#if _DEBUG
  cerr << "StokesCrossCovariance::unload"
    " nbin=" << matrix->get_nbin() <<
    " npol=" << matrix->get_npol() <<
    " nlag=" << matrix->get_nlag() <<
    " ndat=" << matrix->get_data().size() << endl;
#endif
}

void StokesCrossCovariance::Stream::get_data (unsigned off, unsigned ndat, 
                                              double* data) const
{
  assert (off == current_offset);

#if _DEBUG
  cerr << "off=" << off << " ilag=" << ilag << " ibin=" << ibin 
       << " jbin=" << jbin << " ipol=" << ipol << " jpol=" << jpol << endl;
#endif

  unsigned nlag = parent->get_nlag();
  unsigned nbin = parent->get_nbin();
  unsigned npol = 4;
  
  unsigned idat = 0;

  bool initialize = false;

  for (; ilag < nlag; ilag ++)
  {
    if (initialize)
      ibin = 0;

    for (; ibin < nbin ; ibin ++)
    {
      // at lag zero, take only the upper triangle
      if (initialize)
        jbin = (ilag == 0) ? ibin : 0;

      for (; jbin < nbin ; jbin ++)
      {
	Matrix<4,4,double> covar;
        covar = parent->get_cross_covariance (ibin, jbin, ilag);

        if (initialize)
          ipol = 0;

	for (; ipol < npol; ipol++)
	{
	  // on the diagonal, take only the upper triangle
          if (initialize)
            jpol = (ilag == 0 && ibin == jbin) ? ipol : 0;

	  while (jpol < npol)
	  {
	    data[idat] = covar[ipol][jpol];
            current_offset ++;
	    idat ++;
	    jpol ++;

            if (idat == ndat)
              return;
	  }

          initialize = true;
	}
      }
    }
  }

  throw Error (InvalidState, "StokesCrossCovariance::Stream::get_data", 
               "unexpected end of loop");
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
  unsigned icross = get_icross_check (ibin, jbin, ilag, "get");
  return cross_covariance[icross];
}

Matrix<4,4,double>&
StokesCrossCovariance::get_cross_covariance (unsigned ibin,
                                             unsigned jbin,
                                             unsigned ilag)
{
  unsigned icross = get_icross_check (ibin, jbin, ilag, "get");
  return cross_covariance[icross];
}

//
//
//
void StokesCrossCovariance::set_cross_covariance (unsigned ibin,
						  unsigned jbin,
						  const Matrix<4,4,double>& C,
						  bool lock)
{
  set_cross_covariance (ibin, jbin, 0, C, lock);  
}

//
//
//
void StokesCrossCovariance::set_cross_covariance (unsigned ibin,
						  unsigned jbin,
						  unsigned ilag,
						  const Matrix<4,4,double>& C,
						  bool lock)
{
  unsigned icross = get_icross_check (ibin, jbin, ilag, "set");

  if (locked[icross])
    throw Error (InvalidState,
		 "StokesCrossCovariance::set_cross_covariance",
		 "icross=%u ibin=%u jbin=%u, ilag=%u already locked",
		 icross, ibin, jbin, ilag);
  
  cross_covariance[ icross ] = C;

  locked[icross] = lock;
}

std::string full_method_name (const string& method)
{
  return "StokesCrossCovariance::" + method + "_cross_covariance";
}

unsigned StokesCrossCovariance::get_icross_check (unsigned ibin,
						  unsigned jbin,
						  unsigned ilag,
						  const char* method) const
{
  if (ibin >= get_nbin())
    throw Error (InvalidRange, full_method_name(method),
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  if (jbin >= get_nbin())
    throw Error (InvalidRange, full_method_name(method),
		 "jbin=%d >= nbin=%d", ibin, get_nbin());

  if (ilag >= get_nlag())
    throw Error (InvalidRange, full_method_name(method),
		 "ilag=%d >= nlag=%d", ilag, get_nlag());

  unsigned icross = get_icross (ibin, jbin, ilag);
  
  if (icross >= get_ncross_total())
    throw Error (InvalidRange, full_method_name(method),
		 "nbin=%u nlag=%u ibin=%u jbin=%u ilag=%u"
		 " -> icross=%u >= ncross=%u",
		 nbin, nlag, ibin, jbin, ilag, icross, get_ncross_total());

  if (icross >= cross_covariance.size())
    throw Error (InvalidRange, full_method_name(method),
		 "nbin=%u nlag=%u ibin=%u jbin=%u ilag=%u"
		 " -> icross=%u >= cross_covariance.size=%u",
		 nbin, nlag, ibin, jbin, ilag, icross, cross_covariance.size());

  return icross;
}


void StokesCrossCovariance::resize ()
{
  unsigned ncross_total = get_ncross_total();
  
  cross_covariance.resize (ncross_total);
  locked.resize (ncross_total, false);
  
  // verify self-consistency of index calculations
  assert ( get_icross (nbin-1, nbin-1, 0) == get_ncross(0)-1 );

  assert ( get_icross (nbin-1, nbin-1, nlag-1) == ncross_total-1 );
}

void StokesCrossCovariance::set_all (double val)
{
  for (unsigned icross=0; icross < cross_covariance.size(); icross++)
    cross_covariance[icross] = val;
}


