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

    //! Number of mutually consistent points required on either side of step
    unsigned depth;
    
  public:

    //! Default constructor
    RobustStepFinder (float threshold = 3.0)
    {
      step_threshold = threshold;
      depth = 3;
    }

    void set_step_threshold (float val) { step_threshold = val; }
    void process (SystemCalibrator*);

  };
}

#endif
