/***************************************************************************
 *
 *   Copyright (C) 2005-2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SignalPath.h"
#include "Pulsar/SingleAxis.h"

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Instrument.h"
#include "Pulsar/Britton2000.h"

#include "Pulsar/ReceptionModelSolver.h"
#include "Pulsar/VariableBackendEstimate.h"

#include "MEAL/Polar.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Gain.h"
#include "MEAL/Value.h"
#include "MEAL/JonesMueller.h"

#include "templates.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;
using namespace MEAL;
using namespace Calibration;

// #define _DEBUG 1

bool SignalPath::verbose = false;

SignalPath::SignalPath (Pulsar::Calibrator::Type* _type)
{
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the model of the instrument
  //

  type = _type;

  valid = true;
  built = false;

  time_variations_engaged = true;
  step_after_cal = false;
  constant_pulsar_gain = false;
  refcal_through_frontend = true;

  time.signal.connect (&convert, &Calibration::ConvertMJD::set_epoch);
}

void SignalPath::copy (SignalPath* other)
{
  cerr << "SignalPath::copy this=" << this << " other=" << other << endl;
  equation->copy_fit( other->equation );
}

void SignalPath::set_valid (bool f, const char* reason)
{
  valid = f;

  if (!valid && reason && verbose)
    cerr << "SignalPath::set_valid reason: " << reason << endl;
}

void SignalPath::set_response (MEAL::Complex2* x)
{
  response = x;
}

//! Allow the specified response parameter to vary as a function of time 
void SignalPath::set_response_variation (unsigned iparam, 
					 Univariate<Scalar>* function)
{
  if (!built)
    build ();

  response_variation[iparam] = function;
  response_chain->set_constraint (iparam, function);
  convert.signal.connect( function, &Univariate<Scalar>::set_abscissa );
}

//! Get the specified response parameter temporal variation function
const Univariate<Scalar>*
SignalPath::get_response_variation (unsigned iparam) const
{
  if (!response_chain->has_constraint (iparam))
    throw Error (InvalidParam, "SignalPath::get_response_variation",
                 "response iparam=%u is not constrained", iparam);

  const Scalar* func = response_chain->get_constraint (iparam);

  return dynamic_cast<const Univariate<Scalar>*> (func);
}

void SignalPath::set_impurity (MEAL::Real4* x)
{
  impurity = x;
}

void SignalPath::set_basis (MEAL::Complex2* x)
{
  if (response)
    throw Error (InvalidState, "SignalPath::set_basis"
		 "cannot set basis after response is constructed");
  
  basis = x;

  // if the instrument has already been constructed, add the basis to it
  if (instrument && basis)
    *instrument *= basis;
}

void SignalPath::set_solver (ReceptionModel::Solver* s)
{
  solver = s;
  if (equation)
    equation->set_solver (s);
}

//! Set true when the pulsar Stokes parameters have been normalized
void SignalPath::set_constant_pulsar_gain (bool value)
{
  constant_pulsar_gain = value;

  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->set_psr_constant_gain (value);
}

//! Get the measurement equation solver
Calibration::ReceptionModel* SignalPath::get_equation ()
{
  if (!built)
    build ();

  return equation;
}

//! Get the measurement equation solver
const Calibration::ReceptionModel*
SignalPath::get_equation () const
{
  return equation;
}

void SignalPath::set_equation (Calibration::ReceptionModel* e)
{
  if (equation)
    throw Error (InvalidState, "SignalPath::set_equation",
		 "equation already set; cannot be reset after construction");

#ifdef _DEBUG
  cerr << "SignalPath::set_equation " << e << endl;
#endif

  equation = e;
}

//! Get the signal path experienced by the pulsar
const MEAL::Complex2*
SignalPath::get_pulsar_transformation (const MJD& epoch) const
{
  if (!built)
    const_build ();

  return get_backend (epoch) -> get_psr_response ();
}

const MEAL::Complex2* SignalPath::get_transformation () const
{
  if (!built)
    const_build ();

  if (solution_response)
    return solution_response;
  else
    return response;
}

MEAL::Complex2* SignalPath::get_transformation ()
{
  const SignalPath* thiz = this;
  return const_cast<MEAL::Complex2*>( thiz->get_transformation() );
}

void SignalPath::const_build () const
{
  const_cast<SignalPath*>(this)->build();
}

void SignalPath::add_transformation (MEAL::Complex2* xform)
{
#if _DEBUG
  cerr << "SignalPath::add_transformation xform=" << xform << endl;
  MEAL::print(cerr, xform);
#endif
  
  if (!impurity)
  {
#if _DEBUG
    MEAL::Function::very_verbose = true;
#endif

    equation->add_transformation (xform);

#if _DEBUG
    MEAL::Function::very_verbose = false;
#endif
    
    return;
  }

  Reference::To< MEAL::ProductRule<MEAL::Real4> > combination;
  combination = new MEAL::ProductRule<MEAL::Real4>;

  combination->add_model( impurity );
  combination->add_model( new MEAL::JonesMueller (xform) );

  equation->add_transformation( combination );
}

void SignalPath::build () try
{
  if (built)
    return;

  if (!equation)
  {
    if (verbose)
      cerr << "SignalPath::build new ReceptionModel" << endl;
    equation = new Calibration::ReceptionModel;
  }

  if (solver)
    equation->set_solver( solver );
  
  if (!response)
  {
    if (verbose)
      cerr << "SignalPath using " << type->get_name() << endl;

    response = Pulsar::new_transformation (type);
  }

  // embed the response in a chain rule so that any parameter can optionally
  // be made to vary (e.g. as a function of time)
  //
  response_chain = new MEAL::ChainRule<MEAL::Complex2>;
  response_chain->set_model( response );

  //
  // construct the instrumental response shared by pulsar and calibrators
  //
  instrument = new MEAL::ProductRule<MEAL::Complex2>;

  *instrument *= response_chain;

  if (basis)
    *instrument *= basis;

  if (constant_pulsar_gain)
    instrument->set_infit (0, false);

  if (!celestial)
  {
    //
    // use the known transformation from the sky to the receptors
    //
    MEAL::Value<MEAL::Complex2>* sky = new MEAL::Value<MEAL::Complex2>;
    projection.signal.connect (sky, &MEAL::Value<MEAL::Complex2>::set_value);

    celestial = sky;
  }
  
  built = true;

  // backends are added after things are built, start with the first one

  //! Estimate of the backend component of response
  backends.resize(1);
  backends[0] = new_backend (instrument);
}
catch (Error& error)
{
  error += "SignalPath::build";
}

VariableBackendEstimate* SignalPath::new_backend (Complex2* mine)
{
  if (!mine)
    mine = stepeach_pcal->clone();
  
  VariableBackendEstimate* backend = new VariableBackendEstimate (mine);
  
  backend -> set_psr_constant_gain (constant_pulsar_gain);
  backend -> set_cal_backend_only (!refcal_through_frontend);

  if (gain_variation)
    backend -> set_gain_variation (gain_variation->clone());

  if (diff_gain_variation)
    backend -> set_diff_gain_variation (diff_gain_variation->clone());
  
  if (diff_phase_variation)
    backend -> set_diff_phase_variation (diff_phase_variation->clone());

  time.signal.connect (&(backend->convert), &ConvertMJD::set_epoch);
    
  return backend;
}

void SignalPath::add_psr_path (VariableBackendEstimate* backend)
{
  if (!built)
    build ();

  //! Signal path experienced by pulsar signal
  Reference::To< MEAL::ProductRule<MEAL::Complex2> > psr_path;
  psr_path = new MEAL::ProductRule<MEAL::Complex2>;

  IndexedProduct* product = backend->get_psr_response();
  
  psr_path -> add_model( product );
  psr_path -> add_model( celestial );

  add_transformation ( psr_path );

  product->set_index ( equation->get_transformation_index () );

  if (verbose)
    cerr << "SignalPath::add_psr_path index="
	 << product->get_index() << endl;
}

void SignalPath::add_cal_path (VariableBackendEstimate* backend)
{
  if (!built)
    build ();

  Reference::To< MEAL::ProductRule<MEAL::Complex2> > cal_path;
  cal_path = new MEAL::ProductRule<MEAL::Complex2>;

  IndexedProduct* product = backend->get_cal_response();
    
  if (foreach_pcal && product->has_index())
  {
    Reference::To< MEAL::Complex2 > clone = foreach_pcal->clone();
    cal_path->add_model( clone );
  }

  cal_path->add_model( product );

  add_transformation ( cal_path );
  product->set_index (equation->get_transformation_index ());

  if (verbose)
    cerr << "SignalPath::add_cal_path index="
	 << product->get_index() << endl;
}

//! Get the index for the signal path experienced by the reference source
unsigned SignalPath::get_cal_path_index (const MJD& epoch) const
{
  return get_backend(epoch)->get_cal_response()->get_index ();
}

//! Get the index for the signal path experienced by the pulsar
unsigned SignalPath::get_psr_path_index (const MJD& epoch) const
{
  return get_backend(epoch)->get_psr_response()->get_index ();
}
  
void SignalPath::set_foreach_calibrator (const MEAL::Complex2* x)
{
  foreach_pcal = x;
}

void SignalPath::set_stepeach_calibrator (const VariableBackend* backend)
{
  stepeach_pcal = backend;
}

void SignalPath::set_gain_variation (MEAL::Univariate<MEAL::Scalar>* func)
{
  gain_variation = func;

  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->set_gain_variation (func->clone());
}

void SignalPath::set_diff_gain_variation (MEAL::Univariate<MEAL::Scalar>* func)
{
  diff_gain_variation = func;

  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->set_diff_gain_variation (func->clone());
}

void SignalPath::set_diff_phase_variation (MEAL::Univariate<MEAL::Scalar>* func)
{
  diff_phase_variation = func;

  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->set_diff_phase_variation (func->clone());
}

void SignalPath::fix_orientation ()
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "SignalPath::fix_orientation",
		 "cannot fix orientation of response=" + response->get_name());

  // fix the orientation of the first receptor
  physical->set_constant_orientation (true);
}

void SignalPath::update () try
{
#if _DEBUG
  cerr << "SignalPath::update" << endl;
#endif
  
  if (!built)
    return;
  
  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->update ();

  if (backends.size() == 1)
    return;

  // integrate all of the backends into the primary response
  BackendEstimate mean;
  mean.set_response (response);

  for (unsigned i=0; i<backends.size(); i++)
    mean.integrate (backends[i]->get_backend ());

  mean.update();

  // gain, diff_gain, diff_phase that yield identity matrices
  double identity [3] = { 1.0, 0.0, 0.0 };

  for (unsigned iparam=0; iparam < 3; iparam++)
  {
    if (response->get_infit (iparam))
    {
      for (unsigned i=0; i<backends.size(); i++)
      {
        if (backends[i]->get_backend()->get_infit (iparam))
          throw Error (InvalidState, "SignalPath::update",
                       "unexpected backend[%u].infit(%u) = true", i, iparam);

	backends[i]->get_backend()->set_param ( iparam, identity[iparam] );
      }
    }
    else
    {
      for (unsigned i=0; i<backends.size(); i++)
      {
        if (iparam == 0 && constant_pulsar_gain)
          continue;

        if (! backends[i]->get_backend()->get_infit (iparam))
          throw Error (InvalidState, "SignalPath::update",
                       "unexpected backend[%u].infit(%u) = false", i, iparam);

      }
      response->set_param ( iparam, identity[iparam] );
    }
  }
}
catch (Error& error)
{
  throw error += "SignalPath::update";
}

void SignalPath::check_constraints ()
{
  /* for now, do nothing.

  When flux calibrator observations are not available, it is assumed
  that the noise diode has zero circular polarization.

  */

  /*
    instead of fix_last_step, it will be necessary to go through the
    variable gain transformations and ensure that at least one observation
    has been made along each signal path index
  */
}

void SignalPath::fit_gain (bool flag)
{
  response->set_infit (0, flag);
}

void SignalPath::equal_ellipticities ()
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
  {
    polar->set_infit (3, false); // boost along V
    return;
  }

  /*
    ... in the phenomenological parameterization, no boost along
    the Stokes V axis means that the ellipticities are equal.
  */
  Britton2000* bri00 = dynamic_cast<Britton2000*>( response.get() );
  if (bri00)
  {
    bri00->equal_ellipticities();
    return;
  }

  Instrument* van04 = dynamic_cast<Instrument*>( response.get() );
  if (van04)
  {
    van04->equal_ellipticities();
    return;
  }

  throw Error (InvalidState, 
               "SignalPath::equal_ellipticities",
               "don't know how to handle this for the calibrator model");
}

//! Attempt to reduce the number of degrees of freedom in the model
bool SignalPath::reduce_nfree ()
{
  bool reduced = false;

  for (unsigned i=0; i < backends.size(); i++)
    if (backends[i]->reduce_nfree())
      reduced = true;

  return reduced;
}

void copy_param (MEAL::Function* to, const MEAL::Function* from)
{
  // cerr << "copy_param" << endl;
  unsigned nparam = to->get_nparam ();

  if (nparam != from->get_nparam())
    throw Error (InvalidParam, "copy_param", "to.nparam=%d != from.nparam=%d",
		 nparam, from->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++)
    to->set_Estimate( iparam, from->get_Estimate(iparam) );
}


void SignalPath::copy_transformation (const MEAL::Complex2* xform)
{
  // cerr << "SignalPath::copy_transformation" << endl;
  
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( response.get() );
  if (polar)
  {
    const MEAL::Polar* copy = dynamic_cast<const MEAL::Polar*>( xform );
    if (!copy)
      throw Error (InvalidState, "SignalPath::copy_transformation",
		   "solution is not of the required type");

    copy_param( polar, copy );
  }

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (physical)
  {
    const BackendFeed* copy = dynamic_cast<const BackendFeed*>( xform );
    if (!copy)
      throw Error (InvalidState, "SignalPath::copy_transformation",
		   "solution is not of the required type");

    copy_param( physical->get_frontend(), copy->get_frontend() );
  }
}


VariableBackendEstimate* SignalPath::get_backend (const MJD& epoch) const
{
  for (unsigned i=0; i<backends.size(); i++)
    if ( backends[i]->spans (epoch) )
      return backends[i];

  throw Error (InvalidParam, "SignalPath::get_backend",
	       "epoch=" + epoch.printdays(13) + " not spanned");
}

void SignalPath::integrate_calibrator (const MJD& epoch,
				       const MEAL::Complex2* xform) try
{
  get_backend (epoch)->integrate (xform);
}
catch (Error& error)
{
  throw error += "SignalPath::integrate_calibrator";
}

void SignalPath::set_reference_epoch (const MJD& epoch)
{
#ifdef _DEBUG
  cerr << "SignalPath::set_reference_epoch " << epoch << endl;
#endif

  convert.set_reference_epoch( epoch );

  for (unsigned i=0; i < backends.size(); i++)
    backends[i]->update_reference_epoch ();
}

//! Add a step to the gain variations
void SignalPath::add_gain_step (const MJD& mjd)
{
  add_step (mjd);
  set_free (0, mjd);
}

//! Add a step to the differential gain variations
void SignalPath::add_diff_gain_step (const MJD& mjd)
{
  add_step (mjd);
  set_free (1, mjd);
}

//! Add a step to the differential phase variations
void SignalPath::add_diff_phase_step (const MJD& mjd)
{
  add_step (mjd);
  set_free (2, mjd);
}

void SignalPath::add_step (const MJD& mjd,
			   Calibration::VariableBackend* backend)
{
#if _DEBUG
  cerr << "SignalPath::add_step epoch=" << mjd << endl;
#endif
  
  if (backends.size() == 0)
    throw Error (InvalidState, "SignalPath::add_step",
		 "cannot add step when there are no other backends");

  if (backends.size() == 1)
  {
#if _DEBUG
    cerr << "SignalPath::add_step extracting instrument from first backend"
	 << endl;
#endif
    if (!backend)
      backend = stepeach_pcal->clone();
  
    // decouple the original backend from the instrument ...
    backends[0]->set_response (backend);

    // so that the next use of backend is unique
    backend = backend->clone();
    
    // ... then multiply by the instrument
    backends[0]->add_model (instrument);

    /* for each free parameter of backend, disable fit flags in the response
       (without enabling any flags that are already disabled) */

    for (unsigned i=0; i<backend->get_nparam(); i++)
    {
      if (backend->get_infit(i))
	response->set_infit (i, false);
    }
  }
  
  unsigned in_at = 0;
  
  while (in_at < backends.size() - 1)
  {
    if (mjd == backends[in_at]->get_end_time())
      // already inserted
      return;

    else if (mjd < backends[in_at]->get_end_time())
      break;

    in_at++;
  }

  MEAL::Complex2* xform = 0;
  if (backend)
    xform = backend;

  VariableBackendEstimate* middle = new_backend (xform);
  middle->set_start_time (mjd);
  middle->add_model (instrument);

  // the element that will precede the new one to be inserted
  VariableBackendEstimate* before = backends[in_at];
  before->set_end_time (mjd);

  // move the insertion point to follow before
  in_at ++;

  if (in_at < backends.size())
  {
    // the element that will follow the new one to be inserted
    VariableBackendEstimate* after = backends[in_at];
    middle->set_end_time( after->get_start_time() );
  }

  backends.insert (backends.begin()+in_at, middle);

#if _DEBUG
  for (auto backend: backends)
    cerr << "   start=" << backend->get_start_time()
	 << " end=" << backend->get_end_time() << endl;
#endif
}

//! Allow specified parameter to vary freely in step that spans mjd
void SignalPath::set_free (unsigned iparam, const MJD& mjd)
{
  MJD use = mjd;
  use += 30; // seconds

  get_backend(use)->get_backend()->set_infit (iparam, true);
}

void SignalPath::add_observation_epoch (const MJD& epoch)
{
  MJD zero;

  if (min_epoch == zero || epoch < min_epoch) 
    min_epoch = epoch;

  if (max_epoch == zero || epoch > max_epoch) 
    max_epoch = epoch;

  for (unsigned i=0; i < backends.size(); i++)
    backends[i]->add_observation_epoch (epoch);
}

void SignalPath::add_calibrator_epoch (const MJD& epoch)
{
#if _DEBUG
  cerr << "SignalPath::add_calibrator_epoch epoch=" << epoch << endl;
#endif
  
  MJD zero;

  if (convert.get_reference_epoch() == zero)
    convert.set_reference_epoch ( epoch );

  VariableBackendEstimate* backend = get_backend (epoch);
  IndexedProduct* product = backend->get_cal_response();

  // add a step only if the current cal_response has been integrated
  if (product->has_index() && stepeach_pcal)
  {
    if (verbose)
      cerr << "SignalPath::add_calibrator_epoch adding step at epoch="
	   << epoch << endl;

    double half_minute = 30.0; // seconds
    
    MJD step_at = epoch;
    
    if (step_after_cal)
      step_at += half_minute;
    else
      step_at -= half_minute;

    add_step (step_at);

    backend = get_backend (epoch);
    product = backend->get_cal_response();
  }
  
  // it may be necessary to remove this signal path if
  // the add_data step fails and no other calibrator succeeds
  if (!product->has_index() || foreach_pcal)
  {
    if (verbose)
      cerr << "SignalPath::add_calibrator_epoch"
	" add_polncal_backend" << endl;

    add_cal_path (backend);
  }
}

void SignalPath::set_step_after_cal (bool _after)
{
  step_after_cal = _after;
}

void SignalPath::set_refcal_through_frontend (bool flag)
{
  refcal_through_frontend = flag;

  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->set_cal_backend_only (!flag);
}

void SignalPath::engage_time_variations () try
{
  if (time_variations_engaged)
    return;

  time_variations_engaged = true;

  std::map< unsigned, Reference::To<Univariate<Scalar> > >::iterator ptr;
  for (ptr = response_variation.begin(); ptr != response_variation.end(); ptr++)
    response_chain->set_constraint( ptr->first, ptr->second );

  for (unsigned i=0; i < backends.size(); i++)
    backends[i]->engage_time_variations ();
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

  for (unsigned i=0; i < backends.size(); i++)
    backends[i]->disengage_time_variations ();
}
catch (Error& error)
{
  throw error += "SignalPath::disengage_time_variations";
}

#include "MEAL/CongruenceTransformation.h"

void SignalPath::solve () try
{
  engage_time_variations ();

#if _DEBUG
  cerr << "SignalPath::solve" << endl;
  
  for (unsigned i=0; i<backends.size(); i++)
  {
    MEAL::Complex2* xform = backends[i]->get_psr_response();
    cerr << " backend=" << i << " psr response" << endl;
    MEAL::print (cerr, xform);
    
    xform = backends[i]->get_cal_response();
    cerr << " backend=" << i << " cal response" << endl;
    MEAL::print (cerr, xform);
  }
#endif
  
  get_equation()->solve();

  if (impurity)
    for (unsigned i=0; i<impurity->get_nparam(); i++)
    {
      cerr << i << ":" << impurity->get_Estimate(i) << " ";
      if (i%3 == 2)
	cerr << endl;
    }
}
catch (Error& error)
{
  cerr << "SignalPath::solve failure \n\t"
       << error.get_message() << endl;
}

//! return the backend with the maximum weight
VariableBackendEstimate* SignalPath::max_weight_backend ()
{
  VariableBackendEstimate* backend = 0; 
  float maxweight = 0.0;

  for (unsigned i=0; i<backends.size(); i++)
  {
    if (backends[i]->get_weight() > maxweight)
      {
	maxweight = backends[i]->get_weight();
	backend = backends[i];
      }
  }

  if (!backend)
    throw Error (InvalidState, "SignalPath::get_max_weight_backend",
		 "no backend has weight greater than zero");

  return backend;
}
  
void SignalPath::get_covariance( vector<double>& covar, const MJD& epoch ) try
{
  vector< vector<double> > Ctotal;
  get_equation()->get_solver()->get_covariance (Ctotal);

  if (Ctotal.size() != get_equation()->get_nparam())
    throw Error( InvalidState, "SignalPath::get_covariance",
		 "covariance matrix size=%u != nparam=%u",
		 Ctotal.size(), get_equation()->get_nparam() );

  MEAL::Complex2* xform = get_transformation();

  // extract the indeces of the transformation within the measurement equation
  vector< unsigned > imap;
  MEAL::get_imap( get_equation(), xform, imap );

#if _DEBUG
  cerr << "SignalPath::get_covariance before unmapping \n\t"
    "equation.nparam=" << get_equation()->get_nparam() << "\n\t"
    "xform.nparam=" << xform->get_nparam() << "\n\t"
    "imap.size=" << imap.size() << endl;
#endif

  for (unsigned i=0; i < backends.size(); i++)
    backends[i]->unmap_variations (imap, get_equation());
  
  std::map< unsigned, vector< unsigned > > variation_imap;

  std::map< unsigned, Reference::To<Univariate<Scalar> > >::iterator ptr;
  for (ptr = response_variation.begin(); ptr != response_variation.end(); ptr++)
  {
    MEAL::get_imap( get_equation(), ptr->second, variation_imap[ptr->first] );
    ::set_difference (imap, variation_imap[ptr->first]);
  }

  /*
    If the Instrument class ellipticities are set equal to a single
    independent parameter via a ChainRule, then this will need fixing.
  */
  vector<unsigned> ell_imap;

  Instrument* van04 = dynamic_cast<Instrument*>( response.get() );
  if (van04 && van04->has_equal_ellipticities())
  {
    MEAL::get_imap( get_equation(), van04->get_ellipticities(), ell_imap );
    ::set_difference (imap, ell_imap);
    van04->independent_ellipticities();
  }

  disengage_time_variations (epoch);

#if _DEBUG
  cerr << "SignalPath::get_covariance after unmapping \n\t"
    "equation.nparam=" << get_equation()->get_nparam() << "\n\t"
    "xform.nparam=" << xform->get_nparam() << "\n\t"
    "imap.size=" << imap.size() << endl;
#endif
  
  if (xform->get_nparam() != imap.size())
    throw Error (InvalidState, "SignalPath::get_covariance",
		 "nparam=%u != imap.size=%u",
		 xform->get_nparam(), imap.size());

  VariableBackendEstimate* fiducial = max_weight_backend();
  fiducial->compute_covariance (imap, Ctotal);

  if (backends.size() > 1)
  {
    SingleAxis* backend = fiducial->get_backend ();
    solution_response = response->clone ();
    for (unsigned iparam=0; iparam<backend->get_nparam(); iparam++)
      if (!solution_response->get_infit(iparam))
	solution_response->set_param ( iparam, backend->get_param(iparam) );
  }
  
  for (ptr = response_variation.begin(); ptr != response_variation.end(); ptr++)
    MEAL::covariance( ptr->second, imap[ptr->first],
		      variation_imap[ptr->first], Ctotal );

  if (van04 && ell_imap.size() == 1)
  {
    // map the single free paramater onto the two constrained ellipticities
    imap[ van04->get_ellipticity_index(0) ] = ell_imap[0];
    imap[ van04->get_ellipticity_index(1) ] = ell_imap[0];
  }

  unsigned nparam = xform->get_nparam();
  unsigned ncovar = nparam * (nparam+1) / 2;

  covar.resize (ncovar);
  unsigned count = 0;

  for (unsigned i=0; i<nparam; i++)
  {
#ifdef _DEBUG
    cerr << i << ":" << imap[i] << " " << xform->get_param_name(i) << endl;

    if ( !xform->get_infit(i) && Ctotal[imap[i]][imap[i]] != 0 )
      {
	cerr << i << ":" << imap[i] << " " << xform->get_param_name(i)
	     << " fit=" << xform->get_infit(i) << " var="
	     << Ctotal[imap[i]][imap[i]] << endl;
      }
#endif

    for (unsigned j=i; j<nparam; j++)
    {
      assert( count < ncovar );
      covar[count] = Ctotal[imap[i]][imap[j]];
      count ++;
    }
  }

  assert (count == ncovar);
}
catch (Error& error)
{
  throw error += "SignalPath::get_covariance";
}
