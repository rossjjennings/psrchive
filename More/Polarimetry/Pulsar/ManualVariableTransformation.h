//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ManualVariableTransformation_h
#define __Pulsar_ManualVariableTransformation_h

#include "Pulsar/VariableTransformation.h"
#include "Pulsar/ManualPolnCalibrator.h"

namespace Pulsar {

  //! Adapts a VariableTransformation to a ManualPolnCalibrator
  class ManualVariableTransformation : public VariableTransformation
  {
    Reference::To<ManualPolnCalibrator> calibrator;

  public:

    ManualVariableTransformation (ManualPolnCalibrator*);

    //! Get the transformation
    virtual Jones<double> get_transformation ();
  };
}

#endif
