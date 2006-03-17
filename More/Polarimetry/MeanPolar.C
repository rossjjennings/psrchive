/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Calibration/MeanPolar.h"
#include "MEAL/Polar.h"

void Calibration::MeanPolar::update (MEAL::Complex2* model) const
{
  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>(model);
  if (!polar)
    throw Error (InvalidParam, "Calibration::MeanPolar::update",
		 "Complex2 model is not a Polar");

  polar->set_gain (mean_gain.get_Estimate());

  for (unsigned i=0; i<3; i++) {
    polar->set_boostGibbs (i, mean_boostGibbs[i].get_Estimate());
    polar->set_rotationEuler (i, 0.5 * mean_rotationEuler[i].get_Estimate());
  }
}

void Calibration::MeanPolar::integrate (const MEAL::Complex2* model)
{
  const MEAL::Polar* polar = dynamic_cast<const MEAL::Polar*>(model);
  if (!polar)
    throw Error (InvalidParam, "Calibration::MeanPolar::integrate",
		 "Complex2 polar is not a Polar");

  mean_gain += polar->get_gain ();

  for (unsigned i=0; i<3; i++) {
    mean_boostGibbs[i] += polar->get_boostGibbs (i);
    mean_rotationEuler[i] += 2.0 * polar->get_rotationEuler (i);
  }
}

