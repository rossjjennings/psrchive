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

#include "templates.h"

using namespace std;
using namespace MEAL;
using namespace Calibration;

static bool verbose = false;

VariableBackendEstimate::VariableBackendEstimate (const BackendEstimate*)
{
  psr_response = new MEAL::ProductRule<MEAL::Complex2>;
  cal_response = new MEAL::ProductRule<MEAL::Complex2>;
    
  variable_backend = new VariableBackend;
  set_response (variable_backend);
}

//! Set the response that contains the backend
void VariableBackendEstimate::set_response (MEAL::Complex2* xform)
{
  VariableBackend* vb = MEAL::extract<VariableBackend>( xform );
  if (!vb)
    throw Error (InvalidParam, "VariableBackendEstimate::set_response",
		 "xform=" + xform->get_name() +
		 " does not contain a VariableBackend");

  variable_backend = vb;

  psr_response->clear();
  psr_response->add_model (xform);

  cal_response->clear();
  cal_response->add_model (xform);
  
  BackendEstimate::set_response (vb->get_backend());
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
  variable_backend->set_gain_variation( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  gain = function;
}

void VariableBackendEstimate::set_diff_gain (Univariate<Scalar>* function)
{
  variable_backend -> set_diff_gain_variation( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_gain = function;
}

void VariableBackendEstimate::set_diff_phase (Univariate<Scalar>* function)
{
  variable_backend -> set_diff_phase_variation( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_phase = function;
}

void VariableBackendEstimate::update_reference_epoch ()
{
  cerr << "WARNING VariableBackendEstimate::update_reference_epoch"
    " not implemented" << endl;
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

void VariableBackendEstimate::set_psr_constant_gain (bool flag)
{
  if ( flag )
  {
    if (!cal_gain)
    {
      cal_gain = new VariableGain;
      cal_response->add_model (cal_gain);
    }
    
    if (gain)
      cal_gain->set_gain_variation (gain);

    cal_gain->set_gain ( variable_backend->get_gain() );
    variable_backend->set_gain_variation (0);
    variable_backend->set_gain (1.0);
    variable_backend->set_infit (0, false);
  }
  else
  {
    if (cal_gain)
    {
      variable_backend->set_gain ( cal_gain->get_gain() );
      cal_response->remove_model (cal_gain);
      cal_gain = 0;
    }
    
    if (gain)
      variable_backend->set_gain_variation (gain);
  }
}

void VariableBackendEstimate::set_cal_backend_only (bool flag)
{
  if (flag)
  {
    cal_response->clear ();
    if (cal_gain)
      cal_response->add_model (cal_gain);
    cal_response->add_model (variable_backend);
  }
  else
  {
    cal_response->clear ();
    if (cal_gain)
      cal_response->add_model (cal_gain);
    cal_response->add_model (psr_response);
  }
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

    if (!cal_gain)
      cal_gain->set_gain_variation( gain );
    else
      variable_backend->set_gain_variation( gain );
  }

  if (cal_gain)
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
    variable_backend->set_diff_gain_variation( diff_gain );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "engage diff_phase" << endl;
#endif
    variable_backend->set_diff_phase_variation( diff_phase );
  }

#ifdef _DEBUG
  cerr << "after engage nparam = " << variable_backend->get_nparam() << endl;
#endif
}
catch (Error& error)
{
  throw error += "VariableBackendEstimate::engage_time_variations";
}

void VariableBackendEstimate::disengage_time_variations () try
{
#ifdef _DEBUG
  cerr << "before disengage nparam=" << variable_backend->get_nparam() << endl;
#endif

  if (gain)
  {
#ifdef _DEBUG
    cerr << "disengage gain" << endl;
#endif

    if (cal_gain)
    {
      cal_gain->set_gain_variation (0);
      cal_gain->set_gain( gain->estimate() );
    }
    else
    {
      variable_backend->set_gain_variation (0);
      variable_backend->set_gain( gain->estimate() );
    }
  }

  if (cal_gain)
    variable_backend->set_gain( cal_gain->get_gain() );

  if (diff_gain)
  {
#ifdef _DEBUG
    cerr << "disengage diff_gain value=" << diff_gain->estimate() << endl;
#endif
    variable_backend->set_diff_gain_variation (0);
    variable_backend->set_diff_gain( diff_gain->estimate() );
  }

  if (diff_phase)
  {
#ifdef _DEBUG
    cerr << "disengage diff_phase value=" << diff_phase->estimate() << endl;
#endif
    variable_backend->set_diff_phase_variation (0);
    variable_backend->set_diff_phase( diff_phase->estimate() );
  }

#ifdef _DEBUG
  cerr << "after disengage nparam = " << variable_backend->get_nparam() << endl;
#endif

}
catch (Error& error)
{
  throw error += "VariableBackendEstimate::disengage_time_variations";
}

void VariableBackendEstimate::unmap_variations (vector<unsigned>& imap,
						MEAL::Complex2* composite)
{
  if (gain)
  {
    MEAL::get_imap( composite, gain, gain_imap );
    set_difference (imap, gain_imap);
  }
  else if (cal_gain)
    MEAL::get_imap( composite, cal_gain, gain_imap );

  if (diff_gain)
  {
    MEAL::get_imap( composite, diff_gain, diff_gain_imap );
    set_difference (imap, diff_gain_imap);
  }

  if (diff_phase)
  {
    MEAL::get_imap( composite, diff_phase, diff_phase_imap );
    set_difference (imap, diff_phase_imap);
  }
}


void VariableBackendEstimate::compute_covariance (vector<unsigned>& imap,
						  vector< vector<double> >& C)
{
  if (gain)
    MEAL::covariance( gain, imap[0], gain_imap, C );
  else if (cal_gain)
    imap[0] = gain_imap[0];

  if (diff_gain)
    MEAL::covariance( diff_gain, imap[1], diff_gain_imap, C );

  if (diff_phase)
    MEAL::covariance( diff_phase, imap[2], diff_phase_imap, C );
}
