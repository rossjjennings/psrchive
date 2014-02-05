/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexCorrelation.h"
#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Vectorize.h"
#include "random.h"

#include <iostream>
#include <vector>

using namespace MEAL;
using namespace std;

// ///////////////////////////////////////////////////////////////////////////
//
// template specializations that enable the use of MEAL::LevenbergMarquardt
//
// ///////////////////////////////////////////////////////////////////////////

namespace MEAL
{
  template<>
  class WeightingScheme<double>
  {
  public:
    
    WeightingScheme (double) {}

    double difference (double estimate, double model)
    { return estimate - model; }
    
    double norm (double x) const
    { return x*x; }
    
    double get_weighted_conjugate (double data) const
    { return data; }
    
    double get_weighted_norm (double data) const
    { return norm(data); }
    
  };

  template<>
  class AbscissaTraits<unsigned>
  {
  public:
    static void apply (ScalarVector& model, unsigned abscissa)
    {
      // cerr << "Abscissa::apply " << abscissa << endl;
      model.set_index(abscissa);
    }
  };

}

unsigned max_iterations = 0;

void runtest ()
{
  // the model of the unit vector to be fit
  MEAL::ComplexCorrelation unit;

  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt<double> fit;

  double max = 1.0;
  double max_norm = 0.9999;

  // the random unit coefficient to which to fit
  std::complex<double> target;
  do 
  {
    random_value (target, max);
  }
  while (norm(target) > max_norm);

  // the random unit coefficient to which to fit
  std::complex<double> guess;
  do 
  {
    random_value (guess, max);
  }
  while (norm(guess) > max_norm);

  unit.set (guess);

  // the "data" for the unit coefficient
  const unsigned ndim = 2;
  vector<unsigned> x (ndim);
  vector<double> y (ndim);

  for (unsigned idim=0; idim<ndim; idim++)
  {
    x[idim] = idim;
    y[idim] = (reinterpret_cast<double*>(&target))[idim];
  }

  float best_chisq = 0.0;
  unsigned iterations = 0;
  unsigned maximum_iterations = 500;
  float convergence_chisq = 1e-12;

  cerr << endl <<
    "*************************************************************\n"
    "FIT to " << target << endl << endl;

  Reference::To<ScalarVector> space = vectorize(&unit);

  try
  {
    best_chisq = fit.init (x, y, *space);
  }
  catch (Error& error)
  {
    throw error += "(init)";
  }

  cerr << "chisq=" << best_chisq << endl;

  for (iterations = 0; iterations < maximum_iterations; iterations++) try
  {
    float chisq = fit.iter (x, y, *space);

    if (chisq < best_chisq)
      best_chisq = chisq;

    if (fit.lamda > 1e9)
      fit.lamda = 1e-4;

    cerr << "ITERATION: " << iterations 
	 << " chisq=" << chisq
	 << " convergence=" << convergence_chisq 
	 << " lamda=" << fit.lamda << endl;

    if (iterations+1 > max_iterations)
      max_iterations = iterations + 1;

    if (chisq < convergence_chisq)
      return;
  }
  catch (Error& error)
  {
    /* Each iterative step includes inversion of the Hessian matrix.
       If this fails, then it is likely singular (i.e. there is an
       ill-constrained free parameter). */

    error << "\n\t" "iteration=" << iterations;
    throw error += "Pulsar::ModeSeparation::solve";
  }

  cerr << "chisq=" << best_chisq
       << " iterations=" << iterations << endl
       << " target=" << target 
       << " norm(target)=" << norm(target) << endl
       << " unit=" << unit.evaluate() 
       << " norm(unit)=" << norm(unit.evaluate()) << endl;

  throw Error (InvalidState, "runtest", "too many iterations");
}



int main (int argc, char** argv) try
{
  MEAL::ComplexCorrelation unit;

  std::vector< std::complex<double> > gradient;
  std::complex<double> value = unit.evaluate( &gradient );

  cerr << "default value=" << value << endl;

  for (unsigned i=0; i<gradient.size(); i++)
    cerr << "grad[" << i << "]=" << gradient[i] << endl;

  for (unsigned i=0; i < 1000; i++)
    runtest ();

  cerr << endl <<
    "ALL TESTS PASSED\n\n"
    "maximum number of required iterations = " << max_iterations << endl;

  return 0;
 }
catch (Error & error)
{
  cerr << "test_ComplexCorrelation: error" << error << endl;
  return -1;
}
