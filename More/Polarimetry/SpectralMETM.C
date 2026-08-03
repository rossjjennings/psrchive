/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectralMETM.h"

//! Constructor
Pulsar::SpectralMETM::SpectralMETM (Calibrator::Type* model)
  : PulsarCalibrator (model)
{
  allchan_equation = new MEAL::ReceptionModel;
  // allchan_phases = new MEAL::PhaseGradients<MEAL::Complex2>;

  SystemCalibrator::set_faraday_rotation (nullptr);
  
  response_fixed.resize(6);
  for (unsigned i=0; i<6; i++)
    response_fixed[i] = i+1;

  allchan_faraday_rotation = faraday = new SpectralFaradayRotation;
  faraday->get_transformation()->set_reference_wavelength(0.0);
}
