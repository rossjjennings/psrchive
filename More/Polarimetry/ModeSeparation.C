/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ModeSeparation.h"

#include "MEAL/OrthogonalModes.h"
#include "MEAL/JonesMueller.h"

#include "MEAL/ProductRule.h"
#include "MEAL/SumRule.h"
#include "MEAL/Cast.h"
#include "MEAL/Union.h"
#include "MEAL/Vectorize.h"
#include "MEAL/ScalarMath.h"

#include "MEAL/LevenbergMarquardt.h"

#include "Pauli.h"

#include <iostream>
#include <assert.h>

using namespace MEAL;
using namespace std;

bool Pulsar::ModeSeparation::verbose = true;

Complex2* product (Scalar* a, Complex2* A)
{
  ProductRule<Complex2>* product = new ProductRule<Complex2>;
  product->add_model( cast<Complex2>( exp(*a).get_expression() ) );
  product->add_model( A );
  return product;
}

void Pulsar::ModeSeparation::init ()
{
  OrthogonalModes* modes = new OrthogonalModes;

  mode_A = modes->get_modeA();
  dof_A = new ScalarParameter;

  mode_B = modes->get_modeB();
  dof_B = new ScalarParameter;

  mode_C = new Coherency;
  dof_C = new ScalarParameter;

  SumRule<Complex2>* mean_sum = new SumRule<Complex2>;

  mean_sum->add_model( modes );
  mean_sum->add_model( mode_C );

  mean = mean_sum;

  SumRule<Real4>* cov_sum = new SumRule<Real4>;
  
  // disjoint A
  cov_sum->add_model( new JonesMueller( product(dof_A,mode_A) ) );
  // disjoint B
  cov_sum->add_model( new JonesMueller( product(dof_B,mode_B) ) );
  // superposition
  cov_sum->add_model( new JonesMueller( product(dof_C,mean_sum) ) );

  covariance = cov_sum;

  Union* join = new Union;
  join->push_back( vectorize(mean_sum) );
  join->push_back( vectorize(cov_sum) );

  space = join;
}

Pulsar::ModeSeparation::ModeSeparation ()
{
  init ();
  cerr << "Pulsar::ModeSeparation dimensions=" << space->size() << endl;
  cerr << "Pulsar::ModeSeparation parameters=" << space->get_nparam() << endl;
}

//! Set the mean Stokes parameters
void Pulsar::ModeSeparation::set_mean (const Stokes<double>& stokes)
{
  obs_mean = convert(stokes);
  mode_A->set_stokes(stokes);
}

//! Set the covariances of the Stokes parameters
void Pulsar::ModeSeparation::set_covariance (const Matrix<4,4,double>& covar)
{
  obs_covariance = covar;
}

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



void Pulsar::ModeSeparation::solve ()
{
  // the engine used to find the chi-squared minimum
  MEAL::LevenbergMarquardt<double> fit;

  // get info from all of the MEAL classes
  // MEAL::Function::verbose = 1;

  // get info from the LevenbergMarquardt algorithm
  // fit.verbose = 3;

  // get info from this method
  bool debug = true;

  const unsigned ndim = space->size();

  if (ndim != 24)
    throw Error (InvalidState, "Pulsar::ModeSeparation::solve",
		 "unexpected problem dimension ndim=%u", ndim);

  vector<unsigned> x (ndim);
  vector<double> y (ndim);

  unsigned idim = 0;

  ScalarMapping< Jones<double> > jmap;
  for (unsigned i=0; i<jmap.ndim(); i++)
  {
    x[idim] = idim;
    y[idim] = jmap.element (obs_mean, i);

    if (verbose)
      cerr << "ScalarMapping Jones idim=" << idim << " x=" << x[idim] << endl;

    idim ++;
  }

  ScalarMapping< Matrix<4,4,double> > Mmap;
  for (unsigned i=0; i<Mmap.ndim(); i++)
  {
    x[idim] = idim;
    y[idim] = Mmap.element (obs_covariance, i);

    if (verbose)
      cerr << "ScalarMapping Mueller idim=" << idim << " x=" << x[idim] << endl;

    idim ++;
  }

  assert (idim == ndim);

  float best_chisq = 0.0;
  unsigned iterations = 0;
  unsigned maximum_iterations = 200;
  float convergence_chisq = 1e-3;
  unsigned nfree = 1;

  if (verbose)
    cerr << "Pulsar::ModeSeparation::solve compute initial fit" << endl;

  try
  {
    best_chisq = fit.init (x, y, *space);
  }
  catch (Error& error)
  {
    throw error += "Pulsar::ModeSeparation::solve (init)";
  }

  fit.lamda = 1e-5;
  fit.lamda_increase_factor = 10;
  fit.lamda_decrease_factor = 0.5;

  if (verbose)
    cerr << "Pulsar::ModeSeparation::solve chisq=" << best_chisq << endl;

  float last_lamda = 0.0;

  unsigned stick_to_steepest_decent = 0;
  unsigned patience = 5;

  for (iterations = 0; iterations < maximum_iterations; iterations++) try
  {
    float chisq = fit.iter (x, y, *space);

    if (chisq < best_chisq)
      best_chisq = chisq;

    if (debug)
      cerr << "ITERATION: " << iterations << endl;

    if (convergence_chisq)
    {
      if (debug)
	cerr << "chisq=" << chisq << " convergence="
	     << convergence_chisq << endl;

      if (chisq < convergence_chisq)
	break;
      else
	continue;
    }

    float delta_chisq = chisq - best_chisq;
    float reduced_chisq = chisq / nfree;

    if (verbose || debug)
      cerr << "chisq=" << chisq << " delta_chisq=" << delta_chisq
           << " reduced_chisq=" << reduced_chisq
	   << " lamda=" << fit.lamda << endl;

    if (fit.lamda == 0.0 && fabs(delta_chisq) < 1.0 && delta_chisq <= 0)
    {
      if (debug)
	cerr << "fit good" << endl;
      break;
    }

    if (fit.lamda == 0.0 && delta_chisq > 0)
    {
      if (debug)
	cerr << "maybe not so good" << endl;
      fit.lamda = last_lamda;

      // count when Newton's method seems to be doing very poorly
      stick_to_steepest_decent ++;
    }

    if (delta_chisq <= 0 && fabs(delta_chisq) < 10)
    {
      if (debug)
	cerr << "fit close" << endl;

      if (stick_to_steepest_decent >= 5)
      {
	if (iterations >= maximum_iterations/2 &&
	    fabs(delta_chisq)/best_chisq < 1e-3)
	{
	  if (debug)
	    cerr << "small change in late stages.  patience="
		 << patience << endl;

	  patience --;

	  if (!patience)
	  {
	    if (debug)
	      cerr << "no more patience" << endl;
	    break;
	  }
	}

	if (debug)
	  cerr << "remain patient!" << endl;
      }
      else
      {
	if (debug)
	  cerr << "go for it!" << endl;
	if (fit.lamda != 0)
	  last_lamda = fit.lamda;
	fit.lamda = 0.0;
      }
    }
  }
  catch (Error& error)
  {
    /* Each iterative step includes inversion of the Hessian matrix.
       If this fails, then it is likely singular (i.e. there is an
       ill-constrained free parameter). */

    error << "\n\t" "iteration=" << iterations;
    throw error += "Pulsar::ModeSeparation::solve";
  }

  if (verbose)
    cerr << "Pulsar::ModeSeparation::solve chisq=" << best_chisq
	 << " iterations=" << iterations << endl;

}
