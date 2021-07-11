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
  public:

    void process (SystemCalibrator*);

  };
}

#endif
