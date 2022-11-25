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

// #define _DEBUG 1

VariableBackendEstimate::VariableBackendEstimate (MEAL::Complex2* response)
{
  psr_response = new IndexedProduct;
  cal_response = new IndexedProduct;
  
  if (!response)
    response = new VariableBackend;

  cal_backend_only = false;
  
  set_response (response);
}

void disable_gain (VariableBackend* variable_backend)
{
  variable_backend->set_gain_variation (0);
  variable_backend->set_gain (1.0);
  variable_backend->set_infit (0, false);
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

#if _DEBUG
  cerr << "VariableBackendEstimate::set_response this=" << this <<
    " xform=" << (void*) xform << " " << xform->get_name() <<
    " variable_backend=" << (void*) variable_backend << endl;
#endif

  psr_response->clear();
  psr_response->add_model (xform);

  cal_response->clear();

  if (cal_gain)
  {
    cal_response->add_model (cal_gain);
    disable_gain (vb);
  }
  
  if (cal_backend_only)
    cal_response->add_model (vb);
  else
    cal_response->add_model (psr_response);

  if (gain_variation)
  {
    if (cal_gain)
      cal_gain->set_gain_variation (gain_variation);
    else
      variable_backend->set_gain_variation (gain_variation);
  }

  if (diff_gain_variation)
    variable_backend->set_diff_gain_variation (diff_gain_variation);

  if (diff_phase_variation)
    variable_backend->set_diff_phase_variation (diff_phase_variation);

#if _DEBUG
  cerr << "VariableBackendEstimate::set_response this=" << this
       << " backend=" << vb->get_backend() << endl;
#endif
    
  BackendEstimate::set_response (vb->get_backend());
}

void VariableBackendEstimate::add_model (MEAL::Complex2* xform)
{
  psr_response->add_model (xform);

  VariableBackend* vb = MEAL::extract<VariableBackend>( xform );
  if (vb && cal_backend_only)
    cal_response->add_model (vb);
  else
    cal_response->add_model (xform);
}

void VariableBackendEstimate::update ()
{
#if _DEBUG
  cerr << "VariableBackendEstimate::update this=" << this << endl;
#endif
  
  BackendEstimate::update ();
  
  SingleAxis* single = variable_backend->get_backend ();
  
  if (gain_variation)
    update( gain_variation, single->get_gain().get_value() );

  if (cal_gain)
  {
    cal_gain->set_gain ( single->get_gain() );
    single->set_gain (1.0);
  }
  
  if (diff_gain_variation)
    update( diff_gain_variation, single->get_diff_gain().get_value() );
    
  if (diff_phase_variation)
    update ( diff_phase_variation, single->get_diff_phase().get_value() );
}

void VariableBackendEstimate::update (MEAL::Scalar* function, double value)
{
  MEAL::Polynomial* polynomial = dynamic_cast<MEAL::Polynomial*>( function );

#if _DEBUG
  cerr << "VariableBackendEstimate::update function=" << function
       << " poly=" << polynomial
       << " value=" << value << endl;
#endif
  
  if (polynomial)
    polynomial->set_param( 0, value );
}

void
VariableBackendEstimate::set_gain_variation (Univariate<Scalar>* function)
{
#if _DEBUG
  cerr << "VariableBackendEstimate::set_gain_variation this=" << this <<
    " variable_backend=" << (void*) variable_backend <<
    " function=" << (void*) function << " " << function->get_name() << endl;
#endif
  
  if (cal_gain)
    cal_gain->set_gain_variation( function );
  else
    variable_backend->set_gain_variation( function );

  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  gain_variation = function;
}

void
VariableBackendEstimate::set_diff_gain_variation (Univariate<Scalar>* function)
{
#if _DEBUG
  cerr << "VariableBackendEstimate::set_diff_gain_variation this=" << this <<
    " variable_backend=" << (void*) variable_backend <<
    " function=" << (void*) function << " " << function->get_name() << endl;
#endif
  
  variable_backend -> set_diff_gain_variation( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_gain_variation = function;
}

void
VariableBackendEstimate::set_diff_phase_variation (Univariate<Scalar>* function)
{
#if _DEBUG
  cerr << "VariableBackendEstimate::set_diff_phase_variation this=" << this <<
    " variable_backend=" << (void*) variable_backend <<
    " function=" << (void*) function << " " << function->get_name() << endl;
#endif
  
  variable_backend -> set_diff_phase_variation( function );
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
  diff_phase_variation = function;
}

void VariableBackendEstimate::update_reference_epoch ()
{
  MJD mid = (min_time + max_time) / 2.0;

#if _DEBUG
  cerr << "VariableBackendEstimate::update_reference_epoch"
    " min=" << min_time << " max=" << max_time << " mid=" << mid << endl;
#endif
  
  convert.set_reference_epoch( mid );
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

  if (gain_variation && decrement_nfree (gain_variation))
    reduced = true;

  if (diff_gain_variation && decrement_nfree (diff_gain_variation))
    reduced = true;

  if (diff_phase_variation && decrement_nfree (diff_phase_variation))
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
    
    cal_gain->set_gain ( variable_backend->get_gain() );
    disable_gain (variable_backend);

    if (gain_variation)
      cal_gain->set_gain_variation (gain_variation);
  }
  else
  {
    if (cal_gain)
    {
      variable_backend->set_gain ( cal_gain->get_gain() );
      cal_response->remove_model (cal_gain);
      cal_gain = 0;
    }
    
    if (gain_variation)
      variable_backend->set_gain_variation (gain_variation);
  }
}

void VariableBackendEstimate::set_cal_backend_only (bool flag)
{
  cal_response->clear ();
  if (cal_gain)
    cal_response->add_model (cal_gain);

  if (flag)
    cal_response->add_model (variable_backend);
  else
    cal_response->add_model (psr_response);

  cal_backend_only = flag;
}

void VariableBackendEstimate::engage_time_variations () try
{
#ifdef _DEBUG
  cerr << "before engage nparam = " << variable_backend->get_nparam() << endl;
#endif

  if (gain_variation) 
  {
#ifdef _DEBUG
    cerr << "engage constant_pulsar_gain=" << bool(cal_gain) << endl;
#endif

    if (!cal_gain)
      cal_gain->set_gain_variation( gain_variation );
    else
      variable_backend->set_gain_variation( gain_variation );
  }

  if (cal_gain)
  {
#ifdef _DEBUG
    cerr << "engage fix variable_backend gain = 1" << endl;
#endif
    disable_gain (variable_backend);
  }

  if (diff_gain_variation)
  {
#ifdef _DEBUG
    cerr << "engage diff_gain" << endl;
#endif
    variable_backend->set_diff_gain_variation( diff_gain_variation );
  }

  if (diff_phase_variation)
  {
#ifdef _DEBUG
    cerr << "engage diff_phase" << endl;
#endif
    variable_backend->set_diff_phase_variation( diff_phase_variation );
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

  if (gain_variation)
  {
#ifdef _DEBUG
    cerr << "disengage gain" << endl;
#endif

    if (cal_gain)
    {
      cal_gain->set_gain_variation (0);
      cal_gain->set_gain( gain_variation->estimate() );
    }
    else
    {
      variable_backend->set_gain_variation (0);
      variable_backend->set_gain( gain_variation->estimate() );
    }
  }

  if (cal_gain)
    variable_backend->set_gain( cal_gain->get_gain() );

  if (diff_gain_variation)
  {
#ifdef _DEBUG
    cerr << "disengage diff_gain value="
	 << diff_gain_variation->estimate() << endl;
#endif
    variable_backend->set_diff_gain_variation (0);
    variable_backend->set_diff_gain( diff_gain_variation->estimate() );
  }

  if (diff_phase_variation)
  {
#ifdef _DEBUG
    cerr << "disengage diff_phase value="
	 << diff_phase_variation->estimate() << endl;
#endif
    variable_backend->set_diff_phase_variation (0);
    variable_backend->set_diff_phase( diff_phase_variation->estimate() );
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
						MEAL::Complex2* composite) try
{
  MEAL::get_imap( composite, get_backend(), backend_imap );
      
  if (gain_variation)
  {
    MEAL::get_imap( composite, gain_variation, gain_imap );
    set_difference (imap, gain_imap);
  }
  else if (cal_gain && get_cal_response()->has_index())
  {
    // cal_gain = calibrator observations modelled with own gain xform
    // get_cal_response()->has_index() = cal signal path integrated in M.E.
    MEAL::get_imap( composite, cal_gain, gain_imap );
  }

  if (diff_gain_variation)
  {
    MEAL::get_imap( composite, diff_gain_variation, diff_gain_imap );
    set_difference (imap, diff_gain_imap);
  }

  if (diff_phase_variation)
  {
    MEAL::get_imap( composite, diff_phase_variation, diff_phase_imap );
    set_difference (imap, diff_phase_imap);
  }
}
catch (Error& error)
{
  throw error += "VariableBackendEstimate::unmap_variations";
}


void VariableBackendEstimate::compute_covariance (vector<unsigned>& imap,
						  vector< vector<double> >& C)
{
  SingleAxis* backend = get_backend();

  if (!backend)
    throw Error (InvalidState, "VariableBackendEstimate::compute_covariance",
                 "no backend");

  if (imap.size() < backend->get_nparam())
    throw Error (InvalidParam, "VariableBackendEstimate::compute_covariance",
                 "imap.size=%u < nparam=%u", imap.size(), backend->get_nparam());

  if (backend_imap.size() < backend->get_nparam())
    throw Error (InvalidState, "VariableBackendEstimate::compute_covariance",
                 "backend_imap.size=%u < nparam=%u", backend_imap.size(), backend->get_nparam());

  for (unsigned iparam=0; iparam < backend->get_nparam(); iparam++)
    if (backend->get_infit(iparam))
      imap[iparam] = backend_imap[iparam];

  if (cal_gain && get_cal_response()->has_index())
  {
    if (gain_imap.size() == 0)
      throw Error (InvalidState, "VariableBackendEstimate::compute_covariance",
                 "cal_gain=true but gain_imap.size == 0");

    imap[0] = gain_imap[0];
  }

  try { 
    if (gain_variation)
      MEAL::covariance( gain_variation, imap[0], gain_imap, C );

    if (diff_gain_variation)
      MEAL::covariance( diff_gain_variation, imap[1], diff_gain_imap, C );

    if (diff_phase_variation)
      MEAL::covariance( diff_phase_variation, imap[2], diff_phase_imap, C );
  }
  catch (Error& error)
  {
    throw error += "VariableBackendEstimate::compute_covariance";
  }
}

