/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyMTM.h"
#include "Pulsar/VariableFaradayRotation.h"

using namespace std;

//! Constructor
Pulsar::FrequencyMTM::FrequencyMTM (Calibrator::Type* model)
  : PulsarCalibrator (model)
{
  allchan_equation = new MEAL::ReceptionModel;

  SystemCalibrator::faraday_rotation = 0;
  
  response_fixed.resize(6);
  for (unsigned i=0; i<6; i++)
    response_fixed[i] = i+1;

  allchan_faraday_rotation = faraday = new AllChanFaradayRotation;
  faraday->get_transformation()->set_reference_wavelength(0.0);
}
