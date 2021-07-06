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

  Pulsar_path = 0;
  ReferenceCalibrator_path = 0;

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
SignalPath::get_transformation () const
{
  if (!built)
    const_build ();

  return response;
}

const MEAL::Complex2*
SignalPath::get_pulsar_transformation () const
{
  if (!built)
    const_build ();

  return pulsar_path;
}

MEAL::Complex2*
SignalPath::get_transformation ()
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
  if (!impurity)
  {
    equation->add_transformation (xform);
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

  //
  // the known transformation from the sky to the receptors
  //
  MEAL::Value<MEAL::Complex2>* sky = new MEAL::Value<MEAL::Complex2>;
  projection.signal.connect (sky, &MEAL::Value<MEAL::Complex2>::set_value);

  // new MEAL::EvaluationTracer<MEAL::Complex2>( known );

  //! Estimate of the backend component of response
  backends.resize(1);
  backends[0] = new_backend ();
  backends[0] -> set_response (instrument);

  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the pulsar
  //

  pulsar_path = new MEAL::ProductRule<MEAL::Complex2>;

  pulsar_path -> add_model( backends[0]->get_psr_response() );
  pulsar_path -> add_model( sky );

  if (!equation)
  {
    if (verbose)
      cerr << "SignalPath::build new ReceptionModel" << endl;
    equation = new Calibration::ReceptionModel;
  }

  add_transformation ( pulsar_path );

  Pulsar_path = equation->get_transformation_index ();

  if (verbose)
    cerr << "SignalPath::build pulsar path="
	 << Pulsar_path << endl;

  if (solver)
    equation->set_solver( solver );

  built = true;
}
catch (Error& error)
{
  error += "SignalPath::build";
}

VariableBackendEstimate* SignalPath::new_backend ()
{
  VariableBackendEstimate* backend = new VariableBackendEstimate;
  
  backend -> set_psr_constant_gain (constant_pulsar_gain);
  backend -> set_cal_backend_only (!refcal_through_frontend);

  return backend;
}

void SignalPath::set_foreach_calibrator (const MEAL::Complex2* x)
{
  foreach_pcal = x;
}

void SignalPath::add_polncal_backend ()
{
  if (!built)
    build ();

  Reference::To< MEAL::ProductRule<MEAL::Complex2> > pcal_path;
  pcal_path = new MEAL::ProductRule<MEAL::Complex2>;

  pcal_path->add_model( backends[0]->get_cal_response() );

  if (foreach_pcal && ReferenceCalibrator_path)
  {
    Reference::To< MEAL::Complex2 > clone = foreach_pcal->clone();
    pcal_path->add_model( clone );
  }
  
  add_transformation ( pcal_path );
  ReferenceCalibrator_path = equation->get_transformation_index ();
}

void SignalPath::fix_orientation ()
{
  if (!built)
    build ();

  BackendFeed* physical = dynamic_cast<BackendFeed*>( response.get() );
  if (!physical)
    throw Error (InvalidState, "SignalPath::fix_orientation",
		 "cannot fix orientation when response=" + response->get_name());

  // fix the orientation of the first receptor
  physical->set_constant_orientation (true);
}

void SignalPath::update () try
{
  if (!built)
    return;
  
  for (unsigned i=0; i<backends.size(); i++)
    backends[i]->update ();
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
  unsigned nparam = to->get_nparam ();

  if (nparam != from->get_nparam())
    throw Error (InvalidParam, "copy_param", "to.nparam=%d != from.nparam=%d",
		 nparam, from->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++)
    to->set_param( iparam, from->get_param(iparam) );
}


void 
SignalPath::copy_transformation (const MEAL::Complex2* xform)
{
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

    copy_param( physical, copy );
  }
}

void SignalPath::integrate_calibrator (const MJD& epoch,
				       const MEAL::Complex2* xform) try
{
  for (unsigned i=0; i<backends.size(); i++)
    if ( backends[i]->spans (epoch) )
    {
      backends[i]->integrate (xform);
      break;
    }
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

void SignalPath::add_step (const MJD& mjd)
{
  if (backends.size() == 0)
    throw Error (InvalidState, "SignalPath::add_step",
		 "cannot add step when there are no other backends");

  if (backends.size() == 1)
  {
    // decouple the original backend from the instrument ...
    backends[0]->set_response (new VariableBackend);
    // ... then multiply by the instrument
    backends[0]->get_psr_response()->add_model (instrument);
  }
  
  unsigned in_at = 0;
  
  while (in_at < backends.size() - 1)
  {
    if (mjd == backends[in_at]->get_end_time())
      // already inserted
      return;

    else if (mjd < backends[in_at]->get_end_time())
      break;
  }

  VariableBackendEstimate* middle = new_backend ();
  middle->set_start_time (mjd);
  middle->get_psr_response()->add_model (instrument);

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
  MJD zero;

  if (convert.get_reference_epoch() == zero)
    convert.set_reference_epoch ( epoch );

  // it may be necessary to remove this signal path if
  // the add_data step fails and no other calibrator succeeds
  if (!get_polncal_path() || foreach_pcal)
  {
    if (verbose)
      cerr << "SignalPath::add_calibrator_epoch"
	" add_polncal_backend" << endl;

    add_polncal_backend();
  }

  // Might need to do something about adding steps here
}

#if 0

  MJD half_minute (0.0, 30.0, 0.0);

  if (step_after_cal)
    time.set_value( epoch+half_minute );
  else
    time.set_value( epoch-half_minute );

  if (gain)
    add_step( gain, convert.get_value() );
  
  if (diff_gain)
    add_step( diff_gain, convert.get_value() );
  
  if (diff_phase)
    add_step( diff_phase, convert.get_value() );
}

#endif


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

void SignalPath::solve () try
{
  engage_time_variations ();

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

void SignalPath::get_covariance( vector<double>& covar, const MJD& epoch )
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

  assert (xform->get_nparam() == imap.size());

  // TO DO: find a fiducial
  if (backends.size())
    backends[0]->compute_covariance (imap, Ctotal);
 
  for (ptr = response_variation.begin(); ptr != response_variation.end(); ptr++)
    compute_covariance( imap[ptr->first], Ctotal, variation_imap[ptr->first], ptr->second );

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

  if (count != ncovar)
    throw Error( InvalidState, "SignalPath::get_covariance",
		 "count=%u != ncovar=%u", count, ncovar );
  
}
