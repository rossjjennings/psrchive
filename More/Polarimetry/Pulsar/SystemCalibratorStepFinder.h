//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SystemCalibratorStepFinder.h

#ifndef __Pulsar_SystemCalibratorStepFinder_H
#define __Pulsar_SystemCalibratorStepFinder_H

#include "Pulsar/SystemCalibrator.h"

class Pulsar::SystemCalibrator::StepFinder : public Reference::Able
{
public:

  virtual void process (SystemCalibrator*) = 0;

protected:
  
  std::vector< std::vector<Calibration::SourceObservation> >&
  get_calibrator_data (SystemCalibrator* sc) { return sc->calibrator_data; }

  std::vector< std::vector<Calibration::CoherencyMeasurementSet> >&
  get_pulsar_data (SystemCalibrator* sc) { return sc->pulsar_data; }

  Reference::Vector<Calibration::SignalPath>&
  get_model (SystemCalibrator* sc) { return sc->model; }
};


#endif
