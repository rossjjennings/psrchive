/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ChiSquared.h"
#include "BoxMuller.h"
#include "random.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

static const unsigned ncoef = 2;  // scale and offset

double normalized_difference[ncoef] = { 0,0 };

static bool verbose = false;

void runtest () try
{
  double max_coeff = 3.0;
  double sigma = 0.1;

  double input_values [ncoef];

  for (unsigned i=0; i<ncoef; i++)
  {
    double value = 0;
    random_value (value, max_coeff);

    if (verbose)
      cerr << "c_" << i << "=" << value << endl;
    input_values[i] = value;
  }

  double in_scale = input_values[0];
  double in_offset = input_values[1];

  double x_min = -1.0;
  double x_max = 1.0;
  unsigned ndat = 200;
  double x_step = (x_max - x_min) / (ndat+1);

  std::vector<double> xval(ndat);  // x-ordinate of data
  std::vector<double> yval(ndat);  // y-ordinate of data
  std::vector<double> wt(ndat);  // weight of data

  BoxMuller gasdev;

  // ofstream os ("lm_test_covar.dat");

  for (unsigned i=0; i<ndat; i++)
  {
    double x = x_min + x_step * i;
    double y = in_offset + in_scale * x + sigma*gasdev();

    xval[i] = x;
    yval[i] = y;
    wt[i] = 1.0/(sigma*sigma);
  }

  Estimate<double> scale, offset;
  weighted_linear_fit (scale, offset, yval, xval, wt);

  if (verbose)
    cerr << "scale=" << scale << " offset=" << offset << endl;

  double diff = input_values[0] - scale.val;
  normalized_difference[0] += diff * diff / scale.var;
  diff = input_values[1] - offset.val;
  normalized_difference[1] += diff * diff / offset.var;
}
catch (const Error& error) {
  cerr << error << endl;
}
catch (...) {
  cerr << "Unhandled exception" << endl;
}

int main ()
{
  unsigned ntest = 100000;
  double tolerance = 0.02;

  cerr << "running " << ntest << " linear least-squares fits" << endl;

  for (unsigned i=0; i<ntest; i++)
    runtest ();

  for (unsigned i=0; i<ncoef; i++)
  {
    double avg = normalized_difference[i]/ntest;
    cerr << i << " normalized error=" << avg << endl;

    if ( fabs(avg - 1.0) > tolerance)
    {
      cerr << "FAIL: uncertainty-normalized error is not close enough to 1" << endl;
      return -1;
    }
  }

  cerr << "Test Passed! (all uncertainty-normalized errors are close to 1)" << endl;
  return 0;
}

