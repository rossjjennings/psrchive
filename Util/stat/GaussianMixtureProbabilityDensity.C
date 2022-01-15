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

double GaussianMixtureProbabilityDensity::get (const vector<double>& dat1,
					       const vector<double>& mean)
{
  unsigned ndat = dat1.size();

  assert (mean.size() == ndat);
  assert (model != NULL);
  
  arma::vec residual (ndat);
  
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

  double likelihood = model->log_p( residual );
   
  return likelihood;
}

