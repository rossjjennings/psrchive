/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ComplexRVM.h"
#include "MEAL/RVM.h"

#include "MEAL/ChainRule.h"
#include "MEAL/VectorRule.h"
#include "MEAL/Gain.h"
#include "MEAL/ComplexCartesian.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarConstant.h"
#include "MEAL/Cast.h"
#include "MEAL/Wrap.h"

using namespace std;

void MEAL::ComplexRVM::set_rvm (RVM* new_rvm)
{
  if (verbose)
    cerr << "MEAL::ComplexRVM::init" << endl;

  rvm = new_rvm;

  ScalarMath N = *(rvm->get_north());
  ScalarMath E = *(rvm->get_east ());

  /*
    WvS - 30 August 2021
    
    N and E are no longer necessarily normalized.

    Performing the normalization here

    1. avoids calling sqrt or atan2 in RVM::set_atan_Psi

    2. ensures that the magnitude of each complex value is orthogonal
    to its phase, thereby preserving the ability to marginalize over
    L_i as in Desvignes et al (2019; see Equations S1 and S2 of the
    Supplementary Material).

  */
  
  ScalarMath norm = N*N + E*E;

  ScalarMath Q = (N*N - E*E) / norm;
  ScalarMath U = 2.0 * N * E / norm;

  ChainRule<Complex>* phase = new ChainRule<Complex>;

  // Set up a complex phase function with phase equal to RVM P.A.
  phase->set_model( new ComplexCartesian );
  phase->set_constraint( 0, modelQ = Q.get_expression() );
  phase->set_constraint( 1, modelU = U.get_expression() );

  // gain->set_verbose (true);
  clear ();

  add_model (gain);
  add_model (phase);
}

class MEAL::ComplexRVM::MaximumLikelihoodGain : public Wrap<Complex>
{
  protected:
    Reference::To<ScalarConstant> valQ, varQ;
    Reference::To<ScalarConstant> valU, varU;
    Reference::To<Complex> gain;

  public:

  MaximumLikelihoodGain (Scalar* modelQ, Scalar* modelU)
  {
    valQ = new ScalarConstant;
    varQ = new ScalarConstant;
    valU = new ScalarConstant;
    varU = new ScalarConstant;

    // numerator and denominator of Equation (S3) of Desvignes et al (2019)
    ScalarMath num = (*valQ * *modelQ) / *varQ + (*valU * *modelU) / *varU;
    ScalarMath den = (*modelQ * *modelQ) / *varQ + (*modelU * *modelU) / *varU;
    ScalarMath result = num / den;
 
    Scalar* expression = result.get_expression();
    wrap (new Cast<Complex,Scalar> (expression));
  }

  std::string get_name () const { return "ComplexRVM::MaximumLikelihoodGain"; }

  void set_measured (std::complex< Estimate<double> >& L)
  {
    valQ->set_value( L.real().val );
    varQ->set_value( L.real().var );
    valU->set_value( L.imag().val );
    varU->set_value( L.imag().var );
  }

};

void MEAL::ComplexRVM::init ()
{
  gain = new VectorRule<Complex>;
}

class MEAL::ComplexRVM::State
{
public:
  double phase_radians;
  Reference::To< Complex > gain;

  State ()
  { 
    phase_radians = 0;
  }
};

MEAL::ComplexRVM::ComplexRVM ()
{
  gains_maximum_likelihood = false;
  init ();
}

//! Copy constructor
MEAL::ComplexRVM::ComplexRVM (const ComplexRVM& copy)
{
  gains_maximum_likelihood = copy.gains_maximum_likelihood;
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ComplexRVM& MEAL::ComplexRVM::operator = (const ComplexRVM& copy)
{
  rvm->copy (copy.rvm);
  return *this;
}

MEAL::ComplexRVM::~ComplexRVM ()
{
}

//! Return the name of the class
string MEAL::ComplexRVM::get_name () const
{
  return "ComplexRVM";
}

//! Return the rotating vector model
MEAL::RVM* MEAL::ComplexRVM::get_rvm ()
{
  return rvm;
}

//! Add a state: phase in turns, L is first guess of linear polarization
void MEAL::ComplexRVM::add_state (double phase, std::complex< Estimate<double> >& linear)
{
  State s;
  s.phase_radians = phase;

  if (!gains_maximum_likelihood)
  {
    auto gain = new Gain<Complex> ();
    double L = ::sqrt( ::norm(linear).val );
    gain->set_gain(L);
    s.gain = gain;
  }
  else
  {
    auto gain = new MaximumLikelihoodGain (modelQ, modelU);
    gain->set_measured (linear);
    s.gain = gain;
  }

  gain->push_back( s.gain );
  state.push_back( s );
}

//! Get the number of states
unsigned MEAL::ComplexRVM::get_nstate () const
{
  return state.size();
}

//! Set the current state for which the model will be evaluated
void MEAL::ComplexRVM::set_state (unsigned i)
{
  check (i, "set_state");
  gain->set_index (i);
  rvm->set_abscissa (state[i].phase_radians);
}

//! Set the phase of the ith state
void MEAL::ComplexRVM::set_phase (unsigned i, double phase)
{
  check (i, "set_phase");
  state[i].phase_radians = phase;
}

//! Get the phase of the ith state
double MEAL::ComplexRVM::get_phase (unsigned i) const
{
  check (i, "get_phase");
  return state[i].phase_radians;
}

//! Set the linear polarization of the ith state
void MEAL::ComplexRVM::set_linear (unsigned i, const Estimate<double>& L)
{
  check (i, "set_linear");

  auto gain = dynamic_cast< Gain<Complex>* > (state[i].gain.get());
  if (gain)
    return gain->set_gain(L);
#if 0
  // for now, silently ignore
  else
    throw Error (InvalidState, "MEAL::ComplexRVM::set_linear",
		    "gain object is not of type Gain<Complex>");
#endif
}

//! Get the linear polarization of the ith state
Estimate<double> MEAL::ComplexRVM::get_linear (unsigned i) const
{
  check (i, "get_linear");

  auto gain = dynamic_cast< Gain<Complex>* > (state[i].gain.get());
  if (gain)
    return gain->get_gain();

  auto mlgain = dynamic_cast< MaximumLikelihoodGain* > (state[i].gain.get());
  if (mlgain)
    return mlgain->evaluate().real();

  throw Error (InvalidState, "MEAL::ComplexRVM::get_linear",
		  "gain object is of unknown type");
}

void MEAL::ComplexRVM::set_gains_infit (bool flag)
{
  if (gains_maximum_likelihood)
    return;

  for (unsigned i=0; i<state.size(); i++)
    state[i].gain->set_infit (0, flag);
}

void MEAL::ComplexRVM::set_gains_maximum_likelihood (bool flag)
{
  if (gain && gain->size() > 0)
    throw Error (InvalidState, "MEAL::ComplexRVM::set_gains_maximum_likelihood",
		 "cannot set flag after states have been created");

  gains_maximum_likelihood = flag;
}

#if 0
void MEAL::ComplexRVM::renormalize (double renorm)
{
  // MEAL::Function::verbose = true;

  for (unsigned i=0; i<state.size(); i++)
  {
    double L = get_linear(i).get_value();

    set_state(i);
    complex<double> result = evaluate ();

    // cerr << "L=" << L << " abs=" << std::abs(result);

    double new_L = renorm * L * L / abs(result);

    set_linear( i, new_L );

    result = evaluate ();

    // cerr << " : new L=" << new_L << " abs=" << abs(result) << endl;
  }
}
#endif

void MEAL::ComplexRVM::check (unsigned i, const char* method) const
{
  if (i >= state.size())
    throw Error (InvalidParam, string("MEAL::ComplexRVM::") + method,
		 "istate=%u >= nstate=%u", i, state.size());
}
