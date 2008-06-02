/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Instrument.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/Feed.h"

#include "Pauli.h"
#include "Error.h"

using namespace std;

void Calibration::Instrument::init ()
{
  backend = new Calibration::SingleAxis;
  backend_chain = new MEAL::ChainRule<MEAL::Complex2>;
  backend_chain->set_model( backend );

  add_model( backend_chain );

  feed = new Calibration::Feed;
  feed_chain = new MEAL::ChainRule<MEAL::Complex2>;
  feed_chain->set_model( feed );

  add_model( feed_chain );
}

Calibration::Instrument::Instrument ()
{
  init ();
}

Calibration::Instrument::Instrument (const Instrument& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const Calibration::Instrument& 
Calibration::Instrument::operator = (const Instrument& s)
{
  if (&s != this)  {
    *backend = *(s.backend);
    *feed = *(s.feed);
  }
  return *this;
}

Calibration::Instrument* Calibration::Instrument::clone () const
{
  return new Instrument (*this);
}

Calibration::Instrument::~Instrument ()
{
  if (verbose)
    cerr << "Calibration::Instrument::dtor" << endl;
}

//! Return the name of the class
string Calibration::Instrument::get_name () const
{
  return "Instrument";
}

//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> Calibration::Instrument::get_gain () const
{
  if (gain_variation)
    return gain_variation->estimate();
  else
    return backend->get_gain ();
}

//! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
Estimate<double> Calibration::Instrument::get_diff_gain () const
{
  if (diff_gain_variation)
    return diff_gain_variation->estimate();
  else
    return backend->get_diff_gain ();
}

//! Get the differential phase, \f$ \phi \f$, in radians
Estimate<double> Calibration::Instrument::get_diff_phase () const
{
  if (diff_phase_variation)
    return diff_phase_variation->estimate();
  else
    return backend->get_diff_phase ();
}

//! Get the orientation
Estimate<double> Calibration::Instrument::get_orientation (unsigned ir) const
{
  if (orientations)
    return orientations->get_value ();
  else
    return feed->get_orientation (ir);
}

//! Get the ellipticity
Estimate<double> Calibration::Instrument::get_ellipticity (unsigned ir) const
{
  if (ellipticities)
    return ellipticities->get_value ();
  else
    return feed->get_ellipticity (ir);
}


//! Set the instrumental gain, \f$ G \f$, in calibrator flux units
void Calibration::Instrument::set_gain (const Estimate<double>& g)
{
  if (gain_variation)
    throw Error (InvalidState, "Calibration::Instrument::set_gain",
		 "cannot set gain when it is constrained by a function");

  backend->set_gain (g);
}

//! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
void Calibration::Instrument::set_diff_gain (const Estimate<double>& gamma)
{
  if (diff_gain_variation)
    throw Error (InvalidState, "Calibration::Instrument::set_diff_gain",
		 "cannot set diff_gain when it is constrained by a function");

  backend->set_diff_gain (gamma);
}
   
//! Set the differential phase, \f$ \phi \f$, in radians
void Calibration::Instrument::set_diff_phase (const Estimate<double>& phi)
{
  if (diff_phase_variation)
    throw Error (InvalidState, "Calibration::Instrument::set_diff_phase",
		 "cannot set diff_phase when it is constrained by a function");

  backend->set_diff_phase (phi);
}

//! Set the orientation
void Calibration::Instrument::set_orientation (unsigned ir,
					       const Estimate<double>& theta)
{
  if (orientations)
    orientations->set_value (theta);
  else
    feed->set_orientation (ir, theta);
}

//! Set the ellipticity
void Calibration::Instrument::set_ellipticity (unsigned ir,
					       const Estimate<double>& chi)
{
  if (ellipticities)
    ellipticities->set_value (chi);
  else
    feed->set_ellipticity (ir, chi);
}

void Calibration::Instrument::equal_ellipticities ()
{
  if (ellipticities)
    return;

  ellipticities = new MEAL::ScalarParameter;
  feed_chain->set_constraint (0, ellipticities);
  feed_chain->set_constraint (2, ellipticities);
}

void Calibration::Instrument::equal_orientations ()
{
  if (orientations)
    return;

  orientations = new MEAL::ScalarParameter;
  feed_chain->set_constraint (1, orientations);
  feed_chain->set_constraint (3, orientations);
}

void Calibration::Instrument::set_cyclic (bool flag)
{
  feed->set_cyclic (flag);
  backend->set_cyclic (flag);
}

Calibration::Feed* Calibration::Instrument::get_feed ()
{
  return feed;
}

Calibration::SingleAxis* Calibration::Instrument::get_backend ()
{
  return backend;
}

const Calibration::Feed* Calibration::Instrument::get_feed () const
{
  return feed;
}

const Calibration::SingleAxis* Calibration::Instrument::get_backend () const
{
  return backend;
}

//! Set the instrumental gain variation
void Calibration::Instrument::set_gain (MEAL::Scalar* function)
{
  gain_variation = function;
  backend_chain->set_constraint (0, function);
}

//! Set the differential gain variation
void Calibration::Instrument::set_diff_gain (MEAL::Scalar* function)
{
  diff_gain_variation = function;
  backend_chain->set_constraint (1, function);
}

//! Set the differential phase variation
void Calibration::Instrument::set_diff_phase (MEAL::Scalar* function)
{
  diff_phase_variation = function;
  backend_chain->set_constraint (2, function);
}

//! Set the instrumental gain variation
const MEAL::Scalar* Calibration::Instrument::get_gain_variation () const
{
  return gain_variation.ptr();
}

//! Set the differential gain variation
const MEAL::Scalar* Calibration::Instrument::get_diff_gain_variation () const
{
  return diff_gain_variation.ptr();
}
   
//! Set the differential phase variation
const MEAL::Scalar* Calibration::Instrument::get_diff_phase_variation () const
{
  return diff_phase_variation.ptr();
}
 
