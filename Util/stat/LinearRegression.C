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
  vector<double> wt (ndat, 1.0);
  vector<double> alpha (ndat, 1.0);

  if (mask)
  {
    for (unsigned idat=0; idat < ndat; idat++)
      if (! (*mask)[idat])
        wt[idat] = 0.0;
  }

  generalized_least_squares (yval, xval, wt, alpha);
}

void LinearRegression::weighted_least_squares 
(
  const vector<double>& yval,
  const vector<double>& xval,
  const vector<double>& wt
)
{
  vector<double> alpha (yval.size(), 1.0);
  generalized_least_squares (yval, xval, wt, alpha);
}
 
void LinearRegression::generalized_least_squares 
(
  const vector<double>& yval,
  const vector<double>& xval,
  const vector<double>& wt,
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
      if (wt[idim] == 0)
        continue;

      orig_alpha_x += alpha[idim] * xval[idim] * wt[idim];
      orig_alpha_2 += alpha[idim] * alpha[idim] * wt[idim];
    }
 
    x_offset = orig_alpha_x / orig_alpha_2;
    weighted_mean_abscissa = x_offset;
  }

  for (unsigned idim=0; idim < ndim; idim++)
  {
    if (wt[idim] == 0)
      continue;

    double xdatum = xval[idim] - x_offset;

    count ++;
    x_y += yval[idim] * xdatum * wt[idim];
    x_2 += xdatum * xdatum * wt[idim];
    alpha_x += alpha[idim] * xdatum * wt[idim];
    alpha_y += alpha[idim] * yval[idim] * wt[idim];
    alpha_2 += alpha[idim] * alpha[idim] * wt[idim];
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

    throw Error (InvalidState, "linear_fit_work", "non-finite scale=%lf count=%u alpha_2=%lf", scale.val, count, alpha_2);
  }

  if ( ! true_math::finite(scale.var) )
    throw Error (InvalidState, "linear_fit_work", "non-finite scale var=%lf", scale.var);

  if (robust_offset)
  {
    vector<double> diff (count);
    unsigned idiff = 0;
  
    for (unsigned idim=0; idim<ndim; idim++)
    {
      if (wt[idim] == 0)
        continue;
    
      double xdatum = xval[idim] - x_offset;

      diff[idiff] = yval[idim]*wt[idim] - scale.val * xdatum*wt[idim];
      idiff ++;
    }
  
    assert (idiff == count);  
    offset.val = median (diff);
  }

  if ( ! true_math::finite(offset.val) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset=%lf", offset.val);

  if ( ! true_math::finite(offset.var) )
    throw Error (InvalidState, "linear_fit_work", "non-finite offset var=%lf", offset.var);

  // cerr << "scale=" << scale << " offset=" << offset << endl;
}
