/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ChiSquared.h"
#include "UnaryStatistic.h"
#include "myfinite.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>
#include <fstream>

using namespace std;

void linear_fit (Estimate<double>& scale, Estimate<double>& offset,
		 const vector<double>& dat1, const vector<double>& dat2,
		 const vector<bool>* mask)
{
  unsigned ndat = dat1.size();
  vector<double> one (ndat, 1.0);
  vector<double> wt (ndat, 1.0);

  if (mask)
  {
    for (unsigned idat=0; idat < ndat; idat++)
      if (! (*mask)[idat])
	wt[idat] = 0.0;
  }

  linear_fit_work (scale, offset, dat1, dat2, one, wt, false);
}

void weighted_linear_fit (Estimate<double>& scale, Estimate<double>& offset,
			  const vector<double>& yval,
			  const vector<double>& xval,
			  const vector<double>& wt)
{
  vector<double> one (yval.size(), 1.0);
  linear_fit_work (scale, offset, yval, xval, one, wt);
}
 
void linear_fit_work (Estimate<double>& scale, Estimate<double>& offset,
		      const vector<double>& dat1,
		      const vector<double>& dat2,
		      const vector<double>& one,
		      const vector<double>& wt,
		      bool robust_offset)
{
  double mu_1 = 0.0;
  double mu_2 = 0.0;
  double covar = 0.0;
  double var_2 = 0.0;
  double wmu_2 = 0.0;
  double norm = 0;

  unsigned ndim = dat1.size ();
  unsigned count = 0;
  
  for (unsigned idim=0; idim < ndim; idim++)
  {
    if (wt[idim] == 0)
      continue;

    count ++;
    mu_1 += dat1[idim] * wt[idim];
    mu_2 += dat2[idim] * wt[idim];
    covar += dat1[idim] * dat2[idim] * wt[idim];
    var_2 += dat2[idim] * dat2[idim] * wt[idim];
    wmu_2 += one[idim] * dat2[idim] * wt[idim];
    norm += one[idim] * one[idim] * wt[idim];
  }
 
  offset.var = var_2 / norm; // numerator

  mu_1 /= norm;
  mu_2 /= norm;
  covar -= mu_1 * wmu_2;
  var_2 -= mu_2 * wmu_2;
  
  scale.val = covar / var_2;
  scale.var = 1.0 / var_2;

  offset.var /= var_2; // denominator

  if ( ! myfinite(scale.val) )
  {
    ofstream out ("linear_fit_work.dat");
    for (unsigned idim=0; idim < ndim; idim++)
      out << idim << " " << dat1[idim] << " " << dat2[idim] << endl;

    throw Error (InvalidState, "linear_fit_work", "non-finite scale=%lf count=%u norm=%lf", scale.val, count, norm);
  }

  if ( ! myfinite(scale.var) )
    throw Error (InvalidState, "linear_fit_work", "non-finite scale var=%lf", scale.var);

  if (robust_offset)
  {
    vector<double> diff (count);
    unsigned idiff = 0;
  
    for (unsigned idim=0; idim<ndim; idim++)
    {
      if (wt[idim] == 0)
	continue;
    
      diff[idiff] = dat1[idim]*wt[idim] - scale.val * dat2[idim]*wt[idim];
      idiff ++;
    }
  
    assert (idiff == count);  
    offset.val = median (diff);
  }
  else
    offset.val = mu_1 - scale.val * mu_2;

  if ( ! myfinite(offset.val) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset=%lf", offset.val);

  if ( ! myfinite(offset.var) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset var=%lf", offset.var);

  // cerr << "scale=" << scale << " offset=" << offset << endl;
}


using namespace BinaryStatistics;

static double sqr (double x) { return x*x; }

ChiSquared::ChiSquared ()
: BinaryStatistic ("chi", "variance of difference")
{
  robust_linear_fit = true;
  max_zap_fraction = 0.5;
  outlier_threshold = 3.0;
}

double ChiSquared::get (const vector<double>& dat1, const vector<double>& dat2) try
{
  assert (dat1.size() == dat2.size());

  unsigned ndat = dat1.size();
  Estimate<double> scale = 1.0;
  Estimate<double> offset = 0.0;
  
  vector<bool> mask (ndat, true);
    
  if (robust_linear_fit)
  {
    unsigned total_zapped = 0;
    unsigned max_zapped = ndat;
    if (max_zap_fraction)
      max_zapped = max_zap_fraction * max_zapped;

    unsigned zapped = 0;
    do
    {
      linear_fit (scale, offset, dat1, dat2, &mask);

      double sigma = 2.0 * outlier_threshold;
      double var = 1 + sqr(scale.val);
      double cut = sqr(sigma) * var;
      
      zapped = 0;
      
      for (unsigned i=0; i<ndat; i++)
      {
	if (!mask[i])
	  continue;

	double residual = dat1[i] - scale.val * dat2[i] - offset.val;
	if ( outlier_threshold > 0 && sqr(residual) > cut )
        {
	  mask[i] = false;
	  zapped ++;
	}
      }
      
      total_zapped += zapped;
    }
    while (zapped && total_zapped < max_zapped);

#if _DEBUG
    if (total_zapped)
      cerr << "chi ndat=" << ndat << " zapped=" << total_zapped
	   << " scale=" << scale << " offset=" << offset << endl;
#endif
  }
  
  double coeff = 0.0;
  residual.resize (ndat);
    
  for (unsigned i=0; i<ndat; i++)
  {
    residual[i] = dat1[i] - scale.val * dat2[i] - offset.val;

    if ( ! myfinite(residual[i]) )
      throw Error (InvalidState, "ChiSquared::get", "non-finite residual[%u]=%f", i, residual[i]);

    coeff += sqr(residual[i]);

    if (!mask[i])
      residual[i] = 0.0;
  }
  
  double retval = coeff / ( ndat * ( 1 + sqr(scale.val) ) );

  // cerr << "chi=" << retval << " scale=" << scale << " offset=" << offset << endl;
  
  return retval;
}
catch (Error& error)
{
  throw error += "ChiSquared::get";
}

