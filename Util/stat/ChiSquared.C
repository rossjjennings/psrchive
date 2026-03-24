/***************************************************************************
 *
 *   Copyright (C) 2021 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ChiSquared.h"
#include "LinearRegression.h"

#include "statutil.h"
#include "true_math.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>
#include <fstream>

using namespace std;

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
      LinearRegression fit;
      fit.ordinary_least_squares (dat1, dat2, &mask);
      scale = fit.scale;
      offset = fit.offset;

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

  // cerr << "ChiSquared::get residual.size=" << residual.size() << endl;

  for (unsigned i=0; i<ndat; i++)
  {
    residual[i] = dat1[i] - scale.val * dat2[i] - offset.val;

    if ( ! true_math::finite(residual[i]) )
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

