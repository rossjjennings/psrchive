/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StandardModel.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/Feed.h"

#include "MEAL/Polynomial.h"
#include "MEAL/Steps.h"

#include <iostream>
#include <assert.h>

using namespace std;

/*! 
  \param britton if true, use the Britton (2000) decomposition of the receiver
*/
Calibration::StandardModel::StandardModel (bool britton)
{
  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the model of the instrument
  //

  if (britton) {
    if (MEAL::Function::verbose)
      cerr << "Calibration::StandardModel Britton" << endl;
    physical = new Calibration::Instrument;
  }
  else {
    if (MEAL::Function::verbose)
      cerr << "Calibration::StandardModel Hamaker" << endl;
    polar = new MEAL::Polar;
  }

  valid = true;
  built = false;

  time.signal.connect (&convert, &Calibration::ConvertMJD::set_epoch);
}

void Calibration::StandardModel::set_feed_transformation (MEAL::Complex2* x)
{
  feed_transformation = x;
}

void
Calibration::StandardModel::set_platform_transformation (MEAL::Complex2* x)
{
  platform_transformation = x;
}

//! Get the measurement equation solver
Calibration::ReceptionModel* Calibration::StandardModel::get_equation ()
{
  if (!built)
    build ();

  return equation;
}

//! Get the signal path experienced by the pulsar
const MEAL::Complex2*
Calibration::StandardModel::get_transformation () const
{
  if (!built)
    const_build ();

  if (polar)
    return polar;

  if (physical)
    return physical;

  return 0;
}

const MEAL::Complex2*
Calibration::StandardModel::get_pulsar_transformation () const
{
  return pulsar_path;
}

MEAL::Complex2*
Calibration::StandardModel::get_transformation ()
{
  const StandardModel* thiz = this;
  return const_cast<MEAL::Complex2*>( thiz->get_transformation() );
}

void Calibration::StandardModel::const_build () const
{
  const_cast<StandardModel*>(this)->build();
}

void Calibration::StandardModel::build ()
{
  if (built)
    return;

  instrument = new MEAL::ProductRule<MEAL::Complex2>;

  if (polar)
    *instrument *= polar;

  if (physical)
    *instrument *= physical;

  if (feed_transformation)
    *instrument *= feed_transformation;

  equation = new Calibration::ReceptionModel;

  ReferenceCalibrator_path = 0;
  FluxCalibrator_path = 0;

  // ////////////////////////////////////////////////////////////////////
  //
  // initialize the signal path seen by the pulsar
  //

  pulsar_path = new MEAL::ProductRule<MEAL::Complex2>;
  *pulsar_path *= instrument;

  if (platform_transformation)
    *pulsar_path *= platform_transformation;

  *pulsar_path *= &parallactic;

  equation->add_transformation ( pulsar_path );
  Pulsar_path = equation->get_transformation_index ();

  time.signal.connect (&parallactic, &Calibration::Parallactic::set_epoch);

  built = true;
}

void Calibration::StandardModel::add_fluxcal_backend ()
{
  if (!built)
    build ();

  if (!physical)
    throw Error (InvalidState,
		 "Calibration::StandardModel::add_fluxcal_backend",
		 "Cannot model flux calibrator with Hamaker model");

  MEAL::ProductRule<MEAL::Complex2>* path = 0;
  path = new MEAL::ProductRule<MEAL::Complex2>;

  fluxcal_backend = new Calibration::SingleAxis;

  *path *= fluxcal_backend;
  *path *= physical->get_feed();

  if (feed_transformation)
    *path *= feed_transformation;

  equation->add_transformation ( path );
  FluxCalibrator_path = equation->get_transformation_index ();
}

void Calibration::StandardModel::add_polncal_backend ()
{
  if (!built)
    build ();

  pcal_path = new MEAL::ProductRule<MEAL::Complex2>;
  *pcal_path *= instrument;

  equation->add_transformation ( pcal_path );
  ReferenceCalibrator_path = equation->get_transformation_index ();
}

void Calibration::StandardModel::fix_orientation ()
{
  if (physical)
    // set the orientation of the first receptor
    physical->set_infit (4, false);

  if (polar)
    // set the orientation of the last rotation
    polar->set_infit (6, false);
}

void Calibration::StandardModel::update ()
{
  if (polar)
    polar_estimate.update (polar);

  if (fluxcal_backend)
    fluxcal_backend_estimate.update (fluxcal_backend);

  if (physical) {

    Calibration::SingleAxis* backend = physical->get_backend();

    physical_estimate.update (backend);

    if (gain)
      update_parameter( gain, backend->get_gain().get_value() );
      
    if (diff_gain)
      update_parameter( diff_gain, backend->get_diff_gain().get_value() );
    
    if (diff_phase)
      update_parameter( diff_phase, backend->get_diff_phase().get_value() );

  }
}

void Calibration::StandardModel::update_parameter (MEAL::Scalar* function,
						   double value)
{
  MEAL::Polynomial* polynomial = dynamic_cast<MEAL::Polynomial*>( function );
  if (polynomial)
    polynomial->set_param( 0, value );
}


void Calibration::StandardModel::check_constraints ()
{
  /* for now, do nothing.  in the future, might implement the code
     that follows.  it currently causes problems down the line
     (equal_ellipticities sets both ellipticity parameters equal to a
     new parameter using the chain rule, which increases the number of
     parameters, and causes MEAL::Function::copy to fail when writing
     out the result). */

  return;

  if (!fluxcal_backend) {

    /*
      Flux calibrator observations are used to constrain the boost
      component of the degeneracy along the Stokes V axis.  If there
      no such observations, then it is assumed that the boost is 
      zero ...
    */
    if (polar)
      polar->set_infit (3, false);

    /*
      ... in the phenomenological parameterization, no boost along
      the Stokes V axis means that the ellipticities are equal.
    */
    if (physical)
      physical->equal_ellipticities();

  }
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
Calibration::StandardModel::copy_transformation (const MEAL::Complex2* xform)
{
  if (polar) {
    const MEAL::Polar* polar_solution;
    polar_solution = dynamic_cast<const MEAL::Polar*>( xform );
    if (polar_solution)
      copy_param( polar, polar_solution );
    else
      throw Error (InvalidState, "StandardModel::set_transformation",
		   "solution is not of the required type");
  }

  if (physical) {
    const Instrument* instrument = dynamic_cast<const Instrument*>( xform );
    if (instrument)
      copy_param( physical, instrument );
    else
      throw Error (InvalidState, "StandardModel::set_transformation",
		   "solution is not of the required type");
  }
}

void 
Calibration::StandardModel::integrate_parameter (MEAL::Scalar* function,
						 double value)
{
  MEAL::Steps* steps = dynamic_cast<MEAL::Steps*>( function );
  if (!steps)
    return;

  unsigned istep = steps->get_step();

  cerr << "StandardModel set step " << istep << endl;

  steps->set_param( istep, value );
}

void 
Calibration::StandardModel::integrate_calibrator (const MEAL::Complex2* xform,
						  bool flux_calibrator)
{
  if (polar) {

    const MEAL::Polar* polar_solution;

    polar_solution = dynamic_cast<const MEAL::Polar*>( xform );

    if (polar_solution)
      polar_estimate.integrate( polar_solution );

  }


  if (physical) {

    const Calibration::SingleAxis* sa;

    sa = dynamic_cast<const Calibration::SingleAxis*>( xform );

    if (!sa)
      return;

    if (flux_calibrator)
      fluxcal_backend_estimate.integrate( sa );
    else {
      physical_estimate.integrate( sa );

      if (gain)
	integrate_parameter( gain, sa->get_gain().get_value() );
      
      if (diff_gain)
	integrate_parameter( diff_gain, sa->get_diff_gain().get_value() );
      
      if (diff_phase)
	integrate_parameter( diff_phase, sa->get_diff_phase().get_value() );
      
    }
  }

}

using namespace MEAL;

void Calibration::StandardModel::set_gain (Univariate<Scalar>* function)
{
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::set_gain",
		 "cannot set gain variation in polar model");

  physical -> set_gain( function );
  function -> set_argument (0, &convert);
  gain = function;
}

void Calibration::StandardModel::set_diff_gain (Univariate<Scalar>* function)
{
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::set_diff_gain",
		 "cannot set gain variation in polar model");

  physical -> set_diff_gain( function );
  function -> set_argument (0, &convert);
  diff_gain = function;
}

void Calibration::StandardModel::set_diff_phase (Univariate<Scalar>* function)
{
  if (!physical)
    throw Error (InvalidState, "Calibration::StandardModel::set_diff_phase",
		 "cannot set diff_phase variation in polar model");

  physical -> set_diff_phase( function );
  function -> set_argument (0, &convert);
  diff_phase = function;
}

void Calibration::StandardModel::set_calibrator_epochs( vector<MJD>& epochs )
{
  for (unsigned i=0; i < epochs.size(); i++) {

    time.set_value( epochs[i] );

    if (gain)
      add_step( gain, convert.get_value() );

    if (diff_gain)
      add_step( diff_gain, convert.get_value() );

    if (diff_phase)
      add_step( diff_phase, convert.get_value() );

  }
}

void Calibration::StandardModel::add_step (Scalar* function, double step)
{
  Steps* steps = dynamic_cast<Steps*> (function);
  if (steps)
    steps->add_step (step);
}

void Calibration::StandardModel::disengage_time_variations (const MJD& epoch)
{
  if (!physical)
    return;

  time.set_value (epoch);

  Univariate<Scalar>* zero = 0;

#ifdef _DEBUG
  cerr << "before disengage nparam = " << physical->get_nparam() << endl;
#endif

  if (gain) {
#ifdef _DEBUG
    cerr << "disengage gain" << endl;
#endif
    physical->set_gain( zero );
    physical->set_gain( gain->estimate() );
  }

  if (diff_gain) {
#ifdef _DEBUG
    cerr << "disengage diff_gain" << endl;
#endif
    physical->set_diff_gain( zero );
    physical->set_diff_gain( diff_gain->estimate() );
  }

  if (diff_phase) {
#ifdef _DEBUG
    cerr << "disengage diff_phase" << endl;
#endif
    physical->set_diff_phase( zero );
    physical->set_diff_phase( diff_phase->estimate() );
  }

#ifdef _DEBUG
  cerr << "after disengage nparam = " << physical->get_nparam() << endl;
#endif

}

void Calibration::StandardModel::compute_covariance
( unsigned index, vector< vector<double> >& covar,
  vector<unsigned>& function_imap, MEAL::Scalar* function )
{
  vector<double> gradient;
  function->evaluate( &gradient );
  unsigned nparam = function->get_nparam();

  for (unsigned i=0; i<covar.size(); i++) {

    assert( covar[index][i] == 0.0 );

    if (i == index) {
      covar[i][i] = function->estimate().get_variance();
      continue;
    }

    double covariance = 0;
    for (unsigned iparam=0; iparam < nparam; iparam++)
      covariance += gradient[iparam] * covar[ function_imap[iparam] ][i];

    covar[index][i] = covar[i][index] = covariance;

  }

}

void Calibration::StandardModel::get_covariance( vector<double>& covar,
						 const MJD& epoch )
{
  vector< vector<double> > Ctotal;
  vector< unsigned > imap;

  get_equation()->get_fit_covariance (Ctotal);

  if (Ctotal.size() != get_equation()->get_nparam())
    throw Error( InvalidState, "Calibration::StandardModel::get_covariance",
		 "covariance matrix size=%u != nparam=%u",
		 Ctotal.size(), get_equation()->get_nparam() );

  vector< unsigned > gain_imap;
  if (gain)
    MEAL::get_imap( get_equation(), gain, gain_imap );

  vector< unsigned > diff_gain_imap;
  if (diff_gain)
    MEAL::get_imap( get_equation(), diff_gain, diff_gain_imap );

  vector< unsigned > diff_phase_imap;
  if (diff_phase)
    MEAL::get_imap( get_equation(), diff_phase, diff_phase_imap );

  vector< unsigned > feed_imap;
  if (physical)
    MEAL::get_imap( get_equation(), physical->get_feed(), feed_imap );

  disengage_time_variations( epoch );

  MEAL::Complex2* xform = get_transformation();

  // extract the indeces of the transformation within the model
  MEAL::get_imap( get_equation(), xform, imap );

  if (gain)
    compute_covariance( imap[0], Ctotal, gain_imap, gain );

  if (diff_gain)
    compute_covariance( imap[1], Ctotal, diff_gain_imap, diff_gain );

  if (diff_phase)
    compute_covariance( imap[2], Ctotal, diff_phase_imap, diff_phase );

  if (physical) {
    unsigned ifeed = 3;
    for (unsigned i=0; i<feed_imap.size(); i++)
      imap[ifeed+i] = feed_imap[i];
  }

  unsigned nparam = xform->get_nparam();
  unsigned ncovar = nparam * (nparam+1) / 2;

  covar.resize (ncovar);
  unsigned count = 0;

  for (unsigned i=0; i<nparam; i++) {
#ifdef _DEBUG
    cerr << i << ":" << imap[i] << " " << xform->get_param_name(i) << endl;
#endif
    for (unsigned j=i; j<nparam; j++) {
      assert( count < ncovar );
      covar[count] = Ctotal[imap[i]][imap[j]];
      count ++;
    }
  }

  if (count != ncovar)
    throw Error( InvalidState, "Calibration::StandardModel::get_covariance",
		 "count=%u != ncovar=%u", count, ncovar );
}
