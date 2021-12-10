/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ChiSquared.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

void linear_fit (double& scale, double& offset,
		 const vector<double>& dat1, const vector<double>& dat2,
		 const vector<bool>* mask)
{
  double covar = 0.0;
  double mu_1 = 0.0;
  double mu_2 = 0.0;
  double var_2 = 0.0;
  unsigned count = 0;
    
  for (unsigned i=0; i<dat1.size(); i++)
  {
    if (mask && !(*mask)[i])
      continue;
      
    mu_1 += dat1[i];
    mu_2 += dat2[i];
    var_2 += dat2[i] * dat2[i];
    covar += dat1[i] * dat2[i];
    
    count ++;
  }

  mu_1 /= count;
  mu_2 /= count;
  covar /= count;
  covar -= mu_1 * mu_2;
  var_2 /= count;
  var_2 -= mu_2 * mu_2;
  
  scale = covar / var_2;

  vector<double> diff (count);
  unsigned idiff = 0;
  
  for (unsigned i=0; i<dat1.size(); i++)
  {
    if (mask && !(*mask)[i])
      continue;
    
    diff[idiff] = dat1[i] - scale * dat2[i];
    idiff ++;
  }
  
  assert (idiff == count);
  
  offset = median (diff);
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

double ChiSquared::get (const vector<double>& dat1, const vector<double>& dat2)
{
  assert (dat1.size() == dat2.size());

  unsigned ndat = dat1.size();
  double scale = 1.0;
  double offset = 0.0;
  
  if (robust_linear_fit)
  {
    vector<bool> mask (ndat, true);
    
    unsigned total_zapped = 0;
    unsigned max_zapped = ndat;
    if (max_zap_fraction)
      max_zapped = max_zap_fraction * max_zapped;

    residual.resize (ndat);
    
    unsigned zapped = 0;
    do
    {
      linear_fit (scale, offset, dat1, dat2, &mask);

      double sigma = 2.0 * outlier_threshold;
      double var = 1 + sqr(scale);
      double cut = sqr(sigma) * var;
      
      zapped = 0;
      
      for (unsigned i=0; i<ndat; i++)
      {
	if (!mask[i])
	  continue;
	
	residual[i] = dat1[i] - scale * dat2[i] - offset;
	if ( outlier_threshold > 0 && sqr(residual[i]) > cut )
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
  for (unsigned i=0; i<ndat; i++)
    coeff += sqr(dat1[i] - scale * dat2[i] - offset);
  
  double retval = coeff / ( ndat * ( 1 + sqr(scale) ) );

  // cerr << "chi=" << retval << endl;
  
  return retval;
}
