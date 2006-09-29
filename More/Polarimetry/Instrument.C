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
  // name = "Instrument";

  backend = new Calibration::SingleAxis;
  // backend->name = "Instrument::backend";
  add_model (backend);

  feed = new Calibration::Feed;
  // feed->name = "Instrument::feed";

  chain = new MEAL::ChainRule<MEAL::Complex2>;
  chain->set_model (feed);

  add_model (chain);
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

Calibration::Instrument::~Instrument ()
{
  if (verbose)
    cerr << "Calibration::Instrument destructor" << endl;
}

//! Return the name of the class
string Calibration::Instrument::get_name () const
{
  return "Instrument";
}

//! Get the instrumental gain, \f$ G \f$, in calibrator flux units
Estimate<double> Calibration::Instrument::get_gain () const
{
  return backend->get_gain ();
}

//! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
Estimate<double> Calibration::Instrument::get_diff_gain () const
{
  return backend->get_diff_gain ();
}

//! Get the differential phase, \f$ \phi \f$, in radians
Estimate<double> Calibration::Instrument::get_diff_phase () const
{
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
  backend->set_gain (g);
}

//! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
void Calibration::Instrument::set_diff_gain (const Estimate<double>& gamma)
{
  backend->set_diff_gain (gamma);
}
   
//! Set the differential phase, \f$ \phi \f$, in radians
void Calibration::Instrument::set_diff_phase (const Estimate<double>& phi)
{
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
  chain->set_constraint (0, ellipticities);
  chain->set_constraint (2, ellipticities);
}

void Calibration::Instrument::equal_orientations ()
{
  if (orientations)
    return;

  orientations = new MEAL::ScalarParameter;
  chain->set_constraint (0, orientations);
  chain->set_constraint (2, orientations);
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
