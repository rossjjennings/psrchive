/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Polynomial.h"
#include "MEAL/LevenbergMarquardt.h"
#include "BoxMuller.h"
#include "random.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace MEAL;
using namespace std;

unsigned max_iterations = 0;

static const unsigned ncoef = 3;

double normalized_difference[ncoef] = { 0,0,0 };

static bool verbose = false;

void runtest () try
{
  double max_coeff = 3.0;
  double sigma = 0.1;

  Polynomial poly (ncoef);
  double input_values [ncoef];

  for (unsigned i=0; i<ncoef; i++)
  {
    double value = 0;
    random_value (value, max_coeff);

    if (verbose)
      cerr << "c_" << i << "=" << value << endl;
    poly.set_param ( i, value );
    input_values[i] = value;
  }

  MEAL::Axis<double> argument; 
  poly.set_argument (0, &argument);

  std::vector< MEAL::Axis<double>::Value > data_x;  // x-ordinate of data
  std::vector< Estimate<double> > data_y;           // y-ordinate of data with error

  double x_min = -1.0;
  double x_max = 1.0;
  unsigned ndat = 200;
  double x_step = (x_max - x_min) / (ndat+1);

  BoxMuller gasdev;

  // ofstream os ("lm_test_covar.dat");

  for (unsigned i=0; i<ndat; i++)
  {
    double x = x_min + x_step * i;
    double y = poly.compute(x) + sigma * gasdev();

    // os << x << " " << y << " " << sigma << endl;

    data_x.push_back( argument.get_Value(x) );
    data_y.push_back( Estimate<double>(y,sigma*sigma) );
  }

  // the engine used to find the chi-squared minimum  
  MEAL::LevenbergMarquardt<double> fit;
  fit.verbose = MEAL::Function::verbose;
  
  float chisq = fit.init (data_x, data_y, poly);

  if (verbose)
    cerr << "initial chisq = " << chisq << endl;
  
  unsigned iter = 1;
  unsigned not_improving = 0;
  double threshold = 0.1;

  while (not_improving < 25)
  {
    if (verbose)
      cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (data_x, data_y, poly);
    if (verbose)
      cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq) {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < threshold && diffchisq > 0)
      {
      	if (verbose)
          cerr << "no big diff in chisq = " << diffchisq << endl;
	      break;
      }
    }
    else
      not_improving ++;

    iter ++;
  }

  double free_parms = data_x.size() - poly.get_nparam();

  if (verbose)
    cerr << "Chi-squared = " << chisq << " / " << free_parms << " = " << chisq / free_parms << endl;

  std::vector<std::vector<double> > covariance;
  fit.result (poly, covariance);

  for (unsigned iparm=0; iparm < poly.get_nparam(); iparm++)
  {
    poly.set_variance (iparm, covariance[iparm][iparm]);

    double diff = poly.get_param(iparm) - input_values[iparm];

    normalized_difference[iparm] += diff * diff / covariance[iparm][iparm];
  }


  string model_text;
  poly.print (model_text);
  if (verbose)
    cout << "ANSWER:\n" << model_text << endl;
}
catch (const Error& error) {
  cerr << error << endl;
}
catch (...) {
  cerr << "Unhandled exception" << endl;
}

int main ()
{
  unsigned ntest = 10000;
  double tolerance = 0.02;

  cerr << "running " << ntest << " quadratic least-squares fits" << endl;

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