//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_VariableTransformationFile_h
#define __Pulsar_VariableTransformationFile_h

#include "Pulsar/KnownVariableTransformation.h"
#include "Pulsar/ManualPolnCalibrator.h"

namespace Pulsar {

  //! Adapts a VariableTransformationManager to a ManualPolnCalibrator
  class VariableTransformationFile : public KnownVariableTransformation
  {
    Reference::To<ManualPolnCalibrator> calibrator;

  public:

    VariableTransformationFile (ManualPolnCalibrator*);

    //! Get the transformation
    LabelledJones<double> get_value () override;
  };
}

#endif
