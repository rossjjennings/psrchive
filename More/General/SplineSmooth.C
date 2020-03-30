/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SplineSmooth.h"
#include "Error.h"

#include <data_table.h>
#include <bspline_builders.h>

using namespace SPLINTER;
using namespace Pulsar;
using namespace std;

double convert (double x) { return x; }

vector<double> convert (const pair<double,double>& x)
{
  vector<double> retval (2);
  retval[0] = x.first;
  retval[1] = x.second;
  return retval;
}

template<typename T>
BSpline SplineSmooth::make_spline (const vector<T>& data_x, 
                                   const vector< Estimate<double> >& data_y)
{
  if (data_x.size() != data_y.size())
    throw Error (InvalidState, "SplineSmooth::make_spline",
                 "data_x.size()=%u != data_y.size()=%u",
                 data_x.size(), data_y.size());

  // Create new DataTable to manage samples
  DataTable samples;
  vector<double> weights;

  for (unsigned i=0; i<data_x.size(); i++)
  {
    samples.add_sample (convert(data_x[i]), data_y[i].val);
    weights.push_back (1.0/data_y[i].var);
  }

  // Build penalized B-spline (P-spline) that smooths the samples
  unsigned int degree = 3;  // cubic
  BSpline::Smoothing smoothing = BSpline::Smoothing::PSPLINE;

  return bspline_smoother(samples, degree, smoothing, alpha, weights);
}

void SplineSmooth1D::set_data (const vector< double >& data_x,
                               const vector< Estimate<double> >& data_y)
{
  pspline = make_spline (data_x, data_y);
}

void SplineSmooth2D::set_data (const vector< std::pair<double,double> >& data_x,
                               const vector< Estimate<double> >& data_y)
{ 
  pspline = make_spline (data_x, data_y);
}


