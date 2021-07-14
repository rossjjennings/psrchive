//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/RobustStepFinder.h

#ifndef __Pulsar_RobustStepFinder_H
#define __Pulsar_RobustStepFinder_H

#include "Pulsar/SystemCalibratorStepFinder.h"

namespace Pulsar
{
  class RobustStepFinder : public SystemCalibrator::StepFinder
  {
    //! multiple of median standard deviation considered to be a step
    float step_threshold;
    
  public:

    //! Default constructor
    RobustStepFinder (float threshold = 1.7) { step_threshold = threshold; }

    void set_step_threshold (float val) { step_threshold = val; }
    void process (SystemCalibrator*);

  };
}

#endif
