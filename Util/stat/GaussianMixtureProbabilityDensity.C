/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "GaussianMixtureProbabilityDensity.h"
#include "GeneralizedChiSquared.h"

#include <cassert>
#include <cmath>

using namespace std;
using namespace BinaryStatistics;

GaussianMixtureProbabilityDensity::GaussianMixtureProbabilityDensity ()
: BinaryStatistic ("gmm", "Gaussian mixture model")
{
  model = 0;
}

unsigned GaussianMixtureProbabilityDensity::get_ngaus () const
{
  if (!model)
    return 0;

  return model->n_gaus();
}

double GaussianMixtureProbabilityDensity::get (const vector<double>& dat1,
					       const vector<double>& mean)
{
  unsigned ndat = dat1.size();

  assert (mean.size() == ndat);
  assert (model != NULL);
  
  std::vector<double> residual (ndat);
  
  if (gcs)
  {
    gcs->get (dat1, mean);
    residual = gcs->get_residual();
  }
  else
  {
    for (unsigned i=0; i<ndat; i++)
      residual[i] = dat1[i] - mean[i];
  }

  unsigned ndims = model->n_dims();

  assert (ndims > 0);

  if (ndims < ndat)
    residual.resize (ndims);

  double likelihood = model->log_p( arma::vec(residual) );
   
  return likelihood;
}

