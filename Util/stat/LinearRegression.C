/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "LinearRegression.h"
#include "statutil.h"
#include "true_math.h"
#include "Error.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>
#include <fstream>

using namespace std;

void LinearRegression::ordinary_least_squares
(
  const vector<double>& yval, 
  const vector<double>& xval,
  const vector<bool>* mask
)
{
  unsigned ndat = yval.size();
  vector<double> weight (ndat, 1.0);
  vector<double> alpha (ndat, 1.0);

  if (mask)
  {
    for (unsigned idat=0; idat < ndat; idat++)
      if (! (*mask)[idat])
        weight[idat] = 0.0;
  }

  generalized_least_squares (yval, xval, weight, alpha);
}

void LinearRegression::weighted_least_squares 
(
  const vector<double>& yval,
  const vector<double>& xval,
  const vector<double>& weight
)
{
  vector<double> alpha (yval.size(), 1.0);
  generalized_least_squares (yval, xval, weight, alpha);
}

void LinearRegression::generalized_least_squares 
(
  const vector<double>& yval,
  const vector<double>& xval,
  const vector<double>& weight,
  const vector<double>& alpha
)
{
  bool done = false;

  masked_weights = weight;

  while (!done)
  {
    least_squares_worker(yval, xval, masked_weights, alpha);

    done = true;

    if (iterative_outlier_threshold > 0.0)
    {
      double Q1=0, Q2=0, Q3=0;
      Q1_Q2_Q3 (residual, Q1, Q2, Q3);

      double IQR = Q3-Q1;
      double diff_min = Q1 - iterative_outlier_threshold * IQR;
      double diff_max = Q3 + iterative_outlier_threshold * IQR;
      
      // cerr << "diff_min=" << diff_min << " diff_max=" << diff_max << endl;

      unsigned idiff = 0;
      unsigned ndim = yval.size ();

      for (unsigned idim=0; idim<ndim; idim++)
      {
        if (masked_weights[idim] == 0)
          continue;
    
        double diff = residual[idim];
        if (diff > diff_max || diff < diff_min)
        {
          masked_weights[idim] = 0;
          done = false;
        }

        idiff ++;
      }
    }
  }
}

void LinearRegression::least_squares_worker
(
  const vector<double>& yval,
  const vector<double>& xval,
  const vector<double>& weight,
  const vector<double>& alpha
)
{
  double alpha_x = 0.0;
  double alpha_y = 0.0;
  double alpha_2 = 0.0;
  double x_y = 0.0;
  double x_2 = 0.0;

  unsigned ndim = yval.size ();
  unsigned count = 0;

  double x_offset = 0.0;

  if (subtract_weighted_mean_abscissa)
  {
    double orig_alpha_x = 0.0;
    double orig_alpha_2 = 0.0;
    for (unsigned idim=0; idim < ndim; idim++)
    {
      if (weight[idim] == 0)
        continue;

      orig_alpha_x += alpha[idim] * xval[idim] * weight[idim];
      orig_alpha_2 += alpha[idim] * alpha[idim] * weight[idim];
    }
 
    x_offset = orig_alpha_x / orig_alpha_2;
    weighted_mean_abscissa = x_offset;
  }

  for (unsigned idim=0; idim < ndim; idim++)
  {
    if (weight[idim] == 0)
      continue;

    double xdatum = xval[idim] - x_offset;

    count ++;
    x_y += yval[idim] * xdatum * weight[idim];
    x_2 += xdatum * xdatum * weight[idim];
    alpha_x += alpha[idim] * xdatum * weight[idim];
    alpha_y += alpha[idim] * yval[idim] * weight[idim];
    alpha_2 += alpha[idim] * alpha[idim] * weight[idim];
  }
 
  double bar_x = alpha_x / alpha_2;
  double bar_y = alpha_y / alpha_2;
  
  scale.var = 1.0 / (x_2 - bar_x * alpha_x);
  scale.val = (x_y - bar_y * alpha_x) * scale.var;

  offset.val = bar_y - scale.val * bar_x;
  offset.var = (x_2 / alpha_2 + x_offset*x_offset) * scale.var;

  if (!subtract_weighted_mean_abscissa)
    weighted_mean_abscissa = bar_x;

  covariance = -bar_x * scale.var;

  if ( ! true_math::finite(scale.val) )
  {
    ofstream out ("linear_fit_work.dat");
    for (unsigned idim=0; idim < ndim; idim++)
      out << idim << " " << yval[idim] << " " << xval[idim] << endl;

    throw Error (InvalidState, "LinearRegression::generalized_least_squares", 
                 "non-finite scale=%lf count=%u alpha_2=%lf", scale.val, count, alpha_2);
  }

  if ( ! true_math::finite(scale.var) )
    throw Error (InvalidState, "LinearRegression::generalized_least_squares",
		 "non-finite scale var=%lf", scale.var);

  if ( ! true_math::finite(offset.val) )
    throw Error (InvalidState, "LinearRegression::generalized_least_squares",
                 "non-finite offset=%lf", offset.val);

  if ( ! true_math::finite(offset.var) )
    throw Error (InvalidState, "LinearRegression::generalized_least_squares",
                 "non-finite offset var=%lf", offset.var);

  nfree = 0;
  chisq = 0.0;

  if (iterative_outlier_threshold > 0)
    residual.resize(count);

  for (unsigned idim=0; idim<ndim; idim++)
  {
    if (weight[idim] == 0)
      continue;
    
    double xdatum = xval[idim] - x_offset;

    double diff = yval[idim] - scale.val*xdatum - offset.val*alpha[idim];
    chisq += diff * diff * weight[idim];

    if (iterative_outlier_threshold > 0)
      residual[nfree] = diff * sqrt(weight[idim]);

    nfree ++;
  }

  assert (nfree == count);

  if ( ! true_math::finite(offset.val) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset=%lf", offset.val);

  if ( ! true_math::finite(offset.var) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset var=%lf", offset.var);
}
