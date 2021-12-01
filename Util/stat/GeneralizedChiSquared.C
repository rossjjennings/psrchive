/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "GeneralizedChiSquared.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

void general_linear_fit (double& scale, double& offset,
			 const vector< vector<double> >& evec,
			 const vector<double>& eval,
			 const vector<double>& dat1,
			 const vector<double>& dat2,
			 const vector<bool>* mask)
{
  unsigned ndim = evec.size();
  assert (ndim == eval.size());

  // principal components
  vector<double> pc1 (ndim, 0.0);
  vector<double> pc2 (ndim, 0.0);
  vector<double> sum (ndim, 0.0);

  for (unsigned idim=0; idim < ndim; idim++)
  {
    for (unsigned i=0; i<dat1.size(); i++)
    {
      if (mask && !(*mask)[i])
	continue;

      pc1[idim] += evec[idim][i] * dat1[i];
      pc2[idim] += evec[idim][i] * dat2[i];
      sum[idim] += evec[idim][i];
    }
  }
  
  double mu_1 = 0.0;
  double mu_2 = 0.0;
  double covar = 0.0;
  double var_2 = 0.0;
  double wmu_2 = 0.0;
  double norm = 0;
    
  for (unsigned idim=0; idim < ndim; idim++)
  {   
    mu_1 += pc1[idim] / eval[idim];
    mu_2 += pc2[idim] / eval[idim];
    covar += pc1[idim] * pc2[idim] / eval[idim];
    var_2 += pc2[idim] * pc2[idim] / eval[idim];
    wmu_2 += sum[idim] * pc2[idim] / eval[idim];
    norm += sum[idim] * sum[idim] / eval[idim];
  }
  
  mu_1 /= norm;
  mu_2 /= norm;
  covar -= mu_1 * wmu_2;
  var_2 -= mu_2 * wmu_2;
  
  scale = covar / var_2;

  offset = mu_1 - scale * mu_2;
}


using namespace BinaryStatistics;

static double sqr (double x) { return x*x; }

GeneralizedChiSquared::GeneralizedChiSquared ()
: BinaryStatistic ("gchi", "variance of difference")
{
  robust_linear_fit = true;
  max_zap_fraction = 0.5;
}

double GeneralizedChiSquared::get (const vector<double>& dat1,
				   const vector<double>& dat2)
{
  assert (dat1.size() == dat2.size());

  unsigned ndim = eigenvectors.size();
  assert (ndim == eigenvalues.size());

  // principal components
  vector<double> pc1 (ndim, 0.0);
  vector<double> pc2 (ndim, 0.0);
  vector<double> sum (ndim, 0.0);

  for (unsigned idim=0; idim < ndim; idim++)
  {
    for (unsigned i=0; i<dat1.size(); i++)
    {
      pc1[idim] += eigenvectors[idim][i] * dat1[i];
      pc2[idim] += eigenvectors[idim][i] * dat2[i];
      sum[idim] += eigenvectors[idim][i];
    }
  }
  
  double scale = 1.0;
  double offset = 0.0;
  
  if (robust_linear_fit)
  {
    vector<bool> mask (dat1.size(), true);
    
    unsigned total_zapped = 0;
    unsigned max_zapped = dat1.size();
    if (max_zap_fraction)
      max_zapped = max_zap_fraction * max_zapped;
    
    unsigned zapped = 0;
    do
    {
      general_linear_fit (scale, offset,
			  eigenvectors, eigenvalues,
			  dat1, dat2, &mask);
      
      double sigma = 2.0;
      double var = 1 + sqr(scale);
      double cut = sqr(sigma) * var;
      
      zapped = 0;
      
      for (unsigned i=0; i<dat1.size(); i++)
      {
	if (!mask[i])
	  continue;

	double residual = 0.0;
	double norm = 0.0;
	
	for (unsigned idim=0; idim<ndim; idim++)
	{
	  double pc1 = eigenvectors[idim][i] * dat1[i];
	  double pc2 = eigenvectors[idim][i] * dat2[i];
	  double sum = eigenvectors[idim][i];
	  norm += sum * sum;
	  
	  residual += sqr(pc1 - scale * pc2 - offset * sum) / eigenvalues[i];
	}
	
	if ( residual > cut * norm )
        {
	  mask[i] = false;
	  zapped ++;
	}
      }
      
      total_zapped += zapped;
    }
    while (zapped && total_zapped < max_zapped);
  }
  
  double coeff = 0.0;
  for (unsigned i=0; i<ndim; i++)
    coeff += sqr(pc1[i] - scale * pc2[i] - offset * sum[i]) / eigenvalues[i];
  
  return coeff / ( dat1.size() * ( 1 + sqr(scale) ) );
}

void GeneralizedChiSquared::set_eigenvectors (const vector<vector<double> >& m)
{
  eigenvectors = m;
}

void GeneralizedChiSquared::set_eigenvalues (const vector<double>& v)
{
  eigenvalues = v;
}
