#include "Pulsar/PolnProfileFit.h"
#include "Pulsar/PolnProfile.h"

#include "Calibration/ReceptionModel.h"
#include "Calibration/Abbreviations.h"
#include "Calibration/Polynomial.h"
#include "Calibration/Phase.h"

//! Default constructor
Pulsar::PolnProfileFit::PolnProfileFit ()
{
  init ();
}

//! Copy constructor
Pulsar::PolnProfileFit::PolnProfileFit (const PolnProfileFit& fit)
{
  init ();
  operator = (fit);
}

//! Assignment operator
Pulsar::PolnProfileFit&
Pulsar::PolnProfileFit::operator = (const PolnProfileFit& fit)
{
  return *this;
}

//! Destructor
Pulsar::PolnProfileFit::~PolnProfileFit ()
{
}

void Pulsar::PolnProfileFit::init ()
{
  // create the linear phase relationship transformation
  phase = new Calibration::Polynomial (2);
  phase -> set_param (0, 0.0);
  phase -> set_infit (0, false);

  phase_axis.connect (phase, &Calibration::Polynomial::set_abscissa);

  Calibration::Complex2Chain* chain = new Calibration::Complex2Chain;
  chain -> set_model (new Calibration::Phase);
  chain -> set_constraint (0, phase);

  phase_xform = chain;
}

//! Set the standard to which observations will be fit
void Pulsar::PolnProfileFit::set_standard (const PolnProfile* standard)
{
  // make a new model and get rid of the old one.

  model->add_transformation ();
}

//! Set the transformation between the standard and observation
void 
Pulsar::PolnProfileFit::set_transformation (Calibration::Complex2* _xform)
{
  xform = _xform;
  if (model)
    model->set_transformation (xform);
}

//! Fit the specified observation to the standard
void Pulsar::PolnProfileFit::fit (const PolnProfile* observation)
{
}

//! Get the phase offset between the observation and the standard
Estimate<double> Pulsar::PolnProfileFit::get_phase () const
{
  return phase -> get_Estimate (1);
}
