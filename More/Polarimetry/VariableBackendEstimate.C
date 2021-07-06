/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableBackendEstimate.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/Polar.h"
#include "MEAL/Polynomial.h"

using namespace std;
using namespace MEAL;
using namespace Calibration;

static bool verbose = false;

VariableBackendEstimate::VariableBackendEstimate (const BackendEstimate*)
{
  variable_backend = new VariableBackend;
  BackendEstimate::set_response (variable_backend);
}

void VariableBackendEstimate::update ()
{
  SingleAxis* single = dynamic_cast<SingleAxis*>( backend.get() );
  if (!single)
    return;

  if (gain)
    update( gain, single->get_gain().get_value() );
  
  if (diff_gain)
    update( diff_gain, single->get_diff_gain().get_value() );
    
  if (diff_phase)
    update ( diff_phase, single->get_diff_phase().get_value() );
}

void VariableBackendEstimate::update (MEAL::Scalar* function,
						   double value)
{
  MEAL::Polynomial* polynomial = dynamic_cast<MEAL::Polynomial*>( function );
  if (polynomial)
    polynomial->set_param( 0, value );
}

void VariableBackendEstimate::set_gain (Univariate<Scalar>* function)
{
  variable_backend->set_gain( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  gain = function;
}

void VariableBackendEstimate::set_diff_gain (Univariate<Scalar>* function)
{
  variable_backend -> set_diff_gain( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_gain = function;
}

void VariableBackendEstimate::set_diff_phase (Univariate<Scalar>* function)
{
  variable_backend -> set_diff_phase( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_phase = function;
}

bool decrement_nfree (MEAL::Scalar* function)
{
  MEAL::Polynomial* poly = dynamic_cast<MEAL::Polynomial*> (function);
  if (poly && poly->get_nparam() > 1)
  {
    poly->resize( poly->get_nparam() - 1 );
    return true;
  }

  return false;
}

//! Attempt to reduce the number of degrees of freedom in the model
bool VariableBackendEstimate::reduce_nfree ()
{
  bool reduced = false;

  if (gain && decrement_nfree (gain))
    reduced = true;

  if (diff_gain && decrement_nfree (diff_gain))
    reduced = true;

  if (diff_phase && decrement_nfree (diff_phase))
    reduced = true;

  return reduced;
}

void VariableBackendEstimate::engage_time_variations () try
{
#ifdef _DEBUG
  cerr << "before engage nparam = " << variable_backend->get_nparam() << endl;
#endif

  if (gain) 
  {
#ifdef _DEBUG
    cerr << "engage constant_pulsar_gain=" << constant_pulsar_gain << endl;
#endif

    if (!constant_pulsar_gain)
      variable_backend->set_gain( gain );
    else if (pcal_gain_chain)
      pcal_gain_chain->set_constraint (0, gain);

  }

  if (pcal_gain)
  {
#ifdef _DEBUG
    cerr << "engage fix variable_backend gain = 1" << endl;
#endif
    variable_backend->set_gain( 1.0 );
  }

  if (diff_gain)
  {
#ifdef _DEBUG
    cerr << "engage diff_gain" << endl;
#endif
    variable_backend->set_diff_gain( diff_gain );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "engage diff_phase" << endl;
#endif
    variable_backend->set_diff_phase( diff_phase );
  }

#ifdef _DEBUG
  cerr << "after engage nparam = " << variable_backend->get_nparam() << endl;
#endif
}
catch (Error& error)
{
  throw error += "SignalPath::engage_time_variations";
}

void SignalPath::disengage_time_variations (const MJD& epoch) 
try
{
#ifdef _DEBUG
  cerr << "DISENGAGE epoch=" << epoch.printdays(16) << endl;
#endif

  if (!time_variations_engaged)
    return;

  time_variations_engaged = false;

  time.set_value (epoch);

  Univariate<Scalar>* zero = 0;

  std::map< unsigned, Reference::To<Univariate<Scalar> > >::iterator ptr;
  for (ptr = response_variation.begin(); ptr != response_variation.end(); ptr++)
  {
    response_chain->set_constraint( ptr->first, zero );
    response->set_Estimate( ptr->first, ptr->second->estimate() );
  }

  BackendFeed* variable_backend = dynamic_cast<BackendFeed*>( response.get() );
  if (!variable_backend)
    return;

#ifdef _DEBUG
  cerr << "before disengage nparam = " << variable_backend->get_nparam() << endl;
#endif

  if (gain)
  {
#ifdef _DEBUG
    cerr << "disengage gain" << endl;
#endif

    if (!constant_pulsar_gain)
    {
      variable_backend->set_gain( zero );
      variable_backend->set_gain( gain->estimate() );
    }
    else if (pcal_gain_chain)
    {
      pcal_gain_chain->set_constraint( 0, zero );
      pcal_gain->set_gain( gain->estimate() );
    }

  }

  if (pcal_gain)
    variable_backend->set_gain( pcal_gain->get_gain() );

  if (diff_gain)
  {
#ifdef _DEBUG
    cerr << "disengage diff_gain value=" << diff_gain->estimate() << endl;
#endif
    variable_backend->set_diff_gain( zero );
    variable_backend->set_diff_gain( diff_gain->estimate() );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "disengage diff_phase value=" << diff_phase->estimate() << endl;
#endif
    variable_backend->set_diff_phase( zero );
    variable_backend->set_diff_phase( diff_phase->estimate() );
  }

#ifdef _DEBUG
  cerr << "after disengage nparam = " << variable_backend->get_nparam() << endl;
#endif

}
catch (Error& error)
{
  throw error += "SignalPath::disengage_time_variations";
}

void unmap_variations (vector<unsigned>& imap, MEAL::Complex2* composite)
{

  if (gain)
  {
    MEAL::get_imap( get_equation(), gain, gain_imap );
    set_difference (imap, gain_imap);
  }
  else if (pcal_gain)
    MEAL::get_imap( get_equation(), pcal_gain, gain_imap );

 
  if (diff_gain)
  {
    MEAL::get_imap( get_equation(), diff_gain, diff_gain_imap );
    set_difference (imap, diff_gain_imap);
  }


  if (diff_phase)
  {
    MEAL::get_imap( get_equation(), diff_phase, diff_phase_imap );
    set_difference (imap, diff_phase_imap);
  }


   void compute_covariance (vector<unsigned>& imap,  vector< vector<double> >& Ctotal)

  if (gain)
    compute_covariance( imap[0], Ctotal, gain_imap, gain );
  else if (pcal_gain)
    imap[0] = gain_imap[0];

  if (diff_gain)
    compute_covariance( imap[1], Ctotal, diff_gain_imap, diff_gain );

  if (diff_phase)
    compute_covariance( imap[2], Ctotal, diff_phase_imap, diff_phase );
