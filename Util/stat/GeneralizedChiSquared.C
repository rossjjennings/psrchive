/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "GeneralizedChiSquared.h"
#include "ChiSquared.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

void general_linear_fit (double& scale, double& offset,
			 const ndArray<2,double>& evec,
			 const ndArray<1,double>& eval,
			 const vector<double>& dat1,
			 const vector<double>& dat2,
			 const vector<bool>* mask)
{
  unsigned ndim = evec.size();
  assert (ndim == eval.size());

  // principal components
  vector<double> pc1 (ndim, 0.0);
  vector<double> pc2 (ndim, 0.0);
  vector<double> one (ndim, 0.0);
  vector<double> wt (ndim, 0.0);
  
  for (unsigned idim=0; idim < ndim; idim++)
  {
    for (unsigned i=0; i<dat1.size(); i++)
    {
      if (mask && !(*mask)[i])
	continue;

      pc1[idim] += evec[idim][i] * dat1[i];
      pc2[idim] += evec[idim][i] * dat2[i];
      one[idim] += evec[idim][i];
      wt[idim] = 1.0/eval[idim];
    }
  }

  linear_fit_work (scale, offset, pc1, pc2, one, wt);
}
  


using namespace BinaryStatistics;

static double sqr (double x) { return x*x; }

GeneralizedChiSquared::GeneralizedChiSquared ()
: BinaryStatistic ("gcs", "generalized chi-squared")
{
  robust_linear_fit = true;
  max_zap_fraction = 0.5;
  outlier_threshold = 0.0;
}

double GeneralizedChiSquared::get (const vector<double>& dat1,
				   const vector<double>& dat2)
{
  assert (dat1.size() == dat2.size());

  unsigned ndat = dat1.size();
  unsigned ndim = eigenvectors.size();
  assert (ndim == eigenvalues.size());

  // principal components
  vector<double> pc1 (ndim, 0.0);
  vector<double> pc2 (ndim, 0.0);
  vector<double> sum (ndim, 0.0);

  for (unsigned idim=0; idim < ndim; idim++)
  {
    for (unsigned i=0; i<ndat; i++)
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
    vector<bool> mask (ndat, true);
    
    unsigned total_zapped = 0;
    unsigned max_zapped = ndat;
    if (max_zap_fraction)
      max_zapped = max_zap_fraction * max_zapped;

    unsigned zapped = 0;
    unsigned iterations = 0;
    do
    {
      general_linear_fit (scale, offset,
			  eigenvectors, eigenvalues,
			  dat1, dat2, &mask);

      iterations ++;
      
      if (outlier_threshold == 0.0)
	break;
      
      double sigma = 2.0 * outlier_threshold;
      double var = 1 + sqr(scale);
      double cut = sqr(sigma) * var;
      
      zapped = 0;
      
      for (unsigned i=0; i<ndat; i++)
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

    if (total_zapped)
      cerr << "gchi ndat=" << ndat << " zapped=" << total_zapped
	   << " iterations=" << iterations
	   << " scale=" << scale << " offset=" << offset << endl;

  }

  residual.resize (ndim);
  
  double coeff = 0.0;
  for (unsigned i=0; i<ndim; i++)
  {
    residual[i] = pc1[i] - scale * pc2[i] - offset * sum[i];
    coeff += residual[i] * residual[i] / eigenvalues[i];

    if (fptr)
    {
      // cerr << "fprintf resid fptr=" << (void*) fptr << endl;
      fprintf (fptr, " %g", residual[i]);
      // cerr << "fprinted" << endl;
    }
  }
  
  double retval = coeff / ( ndim * ( 1 + sqr(scale) ) );
  
  // cerr << "gcs=" << retval << endl;
  
  return retval;
}

