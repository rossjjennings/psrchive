/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ReceptionCalibrator.h"
#include "Pulsar/InstrumentInfo.h"
#include "Pulsar/PolarCalibrator.h"

Pulsar::ReceptionCalibrator::Info*
Pulsar::ReceptionCalibrator::get_Info () const
{
  const_cast<ReceptionCalibrator*>(this)->initialize();

  switch (model_type) {
    
  case Calibrator::Hamaker:
    return new PolarCalibrator::Info (this);
  case Calibrator::Britton:
    return new InstrumentInfo (this);
  default:
    return 0;
    
  }
}

Pulsar::Calibrator::Type Pulsar::ReceptionCalibrator::get_type () const
{
  return model_type;
}
