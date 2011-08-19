/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ModeSeparation.h"

#include "MEAL/ComplexCorrelation.h"
#include "MEAL/SingularCoherency.h"

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

bool Pulsar::ModeSeparation::verbose = false;

Complex2* product (Scalar* a, Complex2* A)
{
  ProductRule<Complex2>* product = new ProductRule<Complex2>;
  product->add_model( cast<Complex2>( a ) ); //exp(*a).get_expression() ) );
  product->add_model( A );
  return product;
}

//! ScalarMapping specialization for an upper diagonal matrix
template<unsigned N, class T, class M=Matrix<N,N,T> >
struct SymmetricMatrix
{
  static inline unsigned ndim ()
  {
    return (N*(N+1)/2);
  }

  static inline T element (const M& t, unsigned idim)
  { 
    unsigned index = idim;
    unsigned irow=0; 
    while (index >= N-irow && irow < N)
    {
      index -= (N-irow);
      irow ++;
    }
    unsigned icol=irow+index;

    /*
      cerr << "idim=" << idim << " irow=" << irow << " icol=" << icol << endl;
      cerr << "element=" << t[irow][icol] << endl;
    */

    return t[irow][icol];
  }
};

//! ScalarMapping specialization converts coherency matrix to Stokes parameters
template<class T>
struct StokesMapping
{
  static inline unsigned ndim ()
  {
    return 4;
  }

  static inline T element (const Jones<T>& rho, unsigned idim)
  {
    Stokes<T> stokes = coherency (rho);
    return stokes[idim];
  }
};

void Pulsar::ModeSeparation::init ()
{
  cross = new CrossCoherency;
  cross->set_correlation( correlation = new ComplexCorrelation );

  SingularCoherency* A = new SingularCoherency;
  cross->set_modeA(A);
  mode_A = A;
  dof_A = new ScalarParameter (1.0);

  SingularCoherency* B = new SingularCoherency;
  cross->set_modeB(B);
  mode_B = B;
  dof_B = new ScalarParameter (1.0);

  cerr << "Pulsar::ModeSeparation::init CrossCoherency::nparam="
       << cross->get_nparam() << endl;

  SumRule<Coherency>* C_sum = new SumRule<Coherency>;
  C_sum->add_model (A);
  C_sum->add_model (B);
  C_sum->add_model (cross);

  cerr << "Pulsar::ModeSeparation::init SumRule<Coherency>::nparam="
       << C_sum->get_nparam() << endl;

  mode_C = C_sum;
  dof_C = new ScalarParameter (1.0);

  SumRule<Complex2>* mean_sum = new SumRule<Complex2>;

  mean_sum->add_model( mode_A );
  mean_sum->add_model( mode_B );
  mean_sum->add_model( mode_C );

  mean = mean_sum;

  SumRule<Real4>* cov_sum = new SumRule<Real4>;
  
  // disjoint A
  cov_sum->add_model( new JonesMueller( product(dof_A,mode_A) ) );
  // disjoint B
  cov_sum->add_model( new JonesMueller( product(dof_B,mode_B) ) );
  // superposition
  cov_sum->add_model( new JonesMueller( product(dof_C,mode_C) ) );

  covariance = cov_sum;

  Union* join = new Union;
  join->push_back( new Vectorize<Complex2,StokesMapping<double> > (mean_sum) );
  join->push_back( new Vectorize<Real4,SymmetricMatrix<4,double> >(cov_sum) );

  space = join;
}

Pulsar::ModeSeparation::ModeSeparation ()
{
  init ();
  cerr << "Pulsar::ModeSeparation dimensions=" << space->size() << endl;
  cerr << "Pulsar::ModeSeparation parameters=" << space->get_nparam() << endl;
}

template<typename T, typename U, template<typename> class C>
C<T> get_value (const C< Estimate<T,U> >& in)
{
  typedef DatumTraits< C<T> > To;
  typedef DatumTraits< C< Estimate<T,U> > > From;

  C<T> out;

  for (unsigned idim=0; idim < To::ndim(); idim++)
    To::element(out, idim) = get_value( From::element(in, idim) );

  return out;
}

template<unsigned N, typename T, typename U>
Vector<N,T> get_value (const Vector< N, Estimate<T,U> >& in)
{
  typedef DatumTraits< Vector< N,T > > To;
  typedef DatumTraits< Vector< N,Estimate<T,U> > > From;

  Vector<N,T> out;

  for (unsigned idim=0; idim < To::ndim(); idim++)
    To::element(out, idim) = get_value( From::element(in, idim) );
}

template<typename T, typename U>
double get_value (const Estimate<T,U>& in)
{
  return in.get_value();
}

//! Set the mean Stokes parameters
void Pulsar::ModeSeparation::set_mean (const Stokes<Estimate<double> >& S)
{
  obs_mean = S;

  Stokes<double> stokes = get_value(S);
  
  Vector<3,double> p = stokes.get_vector();
  double I = stokes.get_scalar();
  double P = norm(p);

  Stokes<double> A (I+P, (I+P)*p/P);
  A *= 0.5;
  mode_A->set_stokes(A);

  cerr << "MODE A=" << mode_A->evaluate() << endl;

  Stokes<double> B (I-P, -(I-P)*p/P);
  B *= 0.5;
  mode_B->set_stokes(B);

  cerr << "MODE B=" << mode_B->evaluate() << endl;

  cerr << "MODE C=" << mode_C->evaluate() << endl;

  cerr << "CROSS=" << cross->evaluate() << endl;

  cerr << "CORRELATION=" << correlation->evaluate() << endl;

  cerr << "Pulsar::ModeSeparation::set_mean S=" << stokes
       << "\n A=" << A
       << "\n B=" << B
       << "\n A+B=" << A+B << endl;
}

//! Set the covariances of the Stokes parameters
void Pulsar::ModeSeparation::set_covariance 
 (const Matrix< 4,4,Estimate<double> >& covar)
{
  obs_covariance = covar;

  cerr << "Pulsar::ModeSeparation::set_covariance C=\n" << covar << endl;

  Jones<double> sum = mode_C->evaluate();

  cerr << "SUM=" << sum << endl;

  double normS = norm(sum);
  cerr << "Frobenius norm=" << normS << endl;

  double var_I = covar[0][0].get_value();
  cerr << "var_I=" << var_I << endl;

  double scale = 1.0 / sqrt(0.5 * normS / var_I);

  cerr << "scale=" << scale << endl;

  dof_A->set_value(0.01*scale);
  dof_B->set_value(0.01*scale);
  dof_C->set_value(scale);

  cerr << "guess covariance=\n" << covariance->evaluate() << endl;

}

// ///////////////////////////////////////////////////////////////////////////
//
// template specializations that enable the use of MEAL::LevenbergMarquardt
//
// ///////////////////////////////////////////////////////////////////////////

namespace MEAL
{
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

  if (ndim != 14)
    throw Error (InvalidState, "Pulsar::ModeSeparation::solve",
		 "unexpected problem dimension ndim=%u", ndim);

  vector<unsigned> x (ndim);
  vector< Estimate<double> > y (ndim);

  unsigned idim = 0;

  for (unsigned i=0; i<4; i++)
  {
    x[idim] = idim;
    y[idim] = obs_mean[i];

    if (verbose)
      cerr << "ScalarMapping Jones i=" << i << " idim=" << x[idim] 
	   << " value=" << y[idim] << endl;

    idim ++;
  }

  SymmetricMatrix<4,Estimate<double> > Mmap;
  for (unsigned i=0; i<Mmap.ndim(); i++)
  {
    x[idim] = idim;
    y[idim] = Mmap.element (obs_covariance, i);

    if (verbose)
      cerr << "ScalarMapping Mueller i=" << i << " idim=" << x[idim]
	   << " value=" << y[idim] << endl;

    idim ++;
  }

  assert (idim == ndim);

  float best_chisq = 0.0;
  unsigned iterations = 0;
  unsigned maximum_iterations = 200;
  float convergence_chisq = 0; // 1e-6;
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

  cerr << "initial covariance=\n" << covariance->evaluate() << endl;

  for (iterations = 0; iterations < maximum_iterations; iterations++) try
  {
    float chisq = fit.iter (x, y, *space);

    if (chisq < best_chisq)
      best_chisq = chisq;

    if (debug)
      cerr << "ITERATION: " << iterations << endl;

    cerr << "A=" << mode_A->evaluate() << endl;
    cerr << "dofA=" << dof_A->evaluate() << endl;

    cerr << "B=" << mode_B->evaluate() << endl;
    cerr << "dofB=" << dof_B->evaluate() << endl;

    cerr << "C=" << mode_C->evaluate() << endl;
    cerr << "dofC=" << dof_C->evaluate() << endl;

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
