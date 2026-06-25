/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyMTM.h"

using namespace std;
using namespace Calibration;

//! Constructor
Pulsar::FrequencyMTM::FrequencyMTM (Calibrator::Type* model)
  : PulsarCalibrator (model)
{
  allchan_equation = new MEAL::ReceptionModel;
}
