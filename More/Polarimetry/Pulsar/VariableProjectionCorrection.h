//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_VariableProjectionCorrection_h
#define __Pulsar_VariableProjectionCorrection_h

#include "Pulsar/VariableTransformation.h"
#include "Pulsar/ProjectionCorrection.h"

namespace Pulsar {

  //! Adapts a VariableTransformation to a ProjectionCorrection
  class VariableProjectionCorrection : public VariableTransformation
  {
    ProjectionCorrection correction;

  public:

    //! Get the transformation
    virtual Jones<double> get_transformation ();
  };
}

#endif
