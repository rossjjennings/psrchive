/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/StandardModel.h"
#include "Calibration/SingleAxis.h"
#include "Calibration/Feed.h"

#include "MEAL/Polynomial.h"

#include <iostream>
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

  differential_phase_order = 0;
  valid = true;
  built = false;
}

void 
Calibration::StandardModel::set_feed_transformation (MEAL::Complex2* x)
{
  feed_transformation = x;
}

void
Calibration::StandardModel::set_platform_transformation (MEAL::Complex2* x)
{
  platform_transformation = x;
}

void
Calibration::StandardModel::set_differential_phase_order (unsigned order)
{
  differential_phase_order = order;
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

  if (differential_phase_order) {

    MEAL::Complex2* operation;
    operation = new MEAL::Rotation(Vector<3, double>::basis(0));

    MEAL::Polynomial* poly;
    poly = new MEAL::Polynomial (differential_phase_order+1);

    poly -> set_infit (0, false);
    poly -> set_argument (0, &convert);

    MEAL::ChainRule<MEAL::Complex2>* backend;
    backend = new MEAL::ChainRule<MEAL::Complex2>;

    backend -> set_model (operation);
    backend -> set_constraint (0, poly);

    *instrument *= backend;
    time.signal.connect (&convert, &Calibration::ConvertMJD::set_epoch);

  }

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
  if (physical)
    physical_estimate.update (physical->get_backend());
  if (fluxcal_backend)
    fluxcal_backend_estimate.update (fluxcal_backend);
}

void 
Calibration::StandardModel::set_transformation (const MEAL::Complex2* xform)
{
  if (polar) {
    const MEAL::Polar* polar_solution;
    polar_solution = dynamic_cast<const MEAL::Polar*>( xform );
    if (polar_solution)
      polar->copy( polar_solution );
  }

  if (physical) {
    const Instrument* instrument = dynamic_cast<const Instrument*>( xform );
    if (instrument)
      physical->copy( instrument );
  }
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

    if (sa) {
      if (flux_calibrator)
	fluxcal_backend_estimate.integrate( sa );
      else
	physical_estimate.integrate( sa );
    }

  }

}
