/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/UnitTangent.h"
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

  class RestoreUnitTangent : public RestorePolicy
  {
  public:
    RestoreUnitTangent (UnitTangent& model) : unit(model) { store(); }
    void store ()
    {
      cerr << "STORE current=" << unit.get_basis(0) << endl;
      cerr << "STORE x0=" << unit.get_param(0) << " x1=" << unit.get_param(1)
	   << endl;
      backup = unit;
    }

    void restore ()
    {
      cerr << "RESTORE from=" << unit.get_basis(0) << endl;
      cerr << "RESTORE to=" << backup.get_basis(0) << endl;
      unit = backup;
    }

  protected:
    UnitTangent& unit;
    UnitTangent backup;
  };

}

unsigned max_iterations = 0;

void runtest ()
{
  // the model of the unit vector to be fit
  MEAL::UnitTangent unit;

  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt<double> fit;
  //fit.restore_policy = new RestoreUnitTangent( unit );

  // the random unit vector to which to fit
  Vector<3,double> target;
  random_vector (target, 1.0);
  target /= norm(target);

  // the "data" for the unit vector
  const unsigned ndim = 3;
  vector<unsigned> x (ndim);
  vector<double> y (ndim);

  for (unsigned idim=0; idim<ndim; idim++)
  {
    x[idim] = idim;
    y[idim] = target[idim];
  }

  float best_chisq = 0.0;
  unsigned iterations = 0;
  unsigned maximum_iterations = 200;
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
       << " iterations=" << iterations << endl;

  throw Error (InvalidState, "runtest", "too many iterations");
}



int main (int argc, char** argv) try
{
  MEAL::UnitTangent unit;

  Vector<3,double> value = unit.evaluate();
  cerr << "default value=" << value << endl;

  unit.set_param(1, 1.0);

  std::vector< Vector<3,double> > gradient;
  value = unit.evaluate( &gradient );

  cerr << "after del1=1.0 value=" << value << endl;
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
  cerr << "test_UnitTangent: error" << error << endl;
  return -1;
}
