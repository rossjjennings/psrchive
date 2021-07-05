//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BackendEstimate.h

#ifndef __Pulsar_VariableBackendEstimate_H
#define __Pulsar_VariableBackendEstimate_H

#include "Pulsar/BackendEstimate.h"
#include "Pulsar/ConvertMJD.h"
#include "MEAL/Scalar.h"

namespace Calibration
{
  //! Manages a variable backend and its best estimate
  class VariableBackendEstimate : public BackendEstimate
  {
  public:

    //! The backend variation transformations
    Reference::To< MEAL::Scalar > gain;
    Reference::To< MEAL::Scalar > diff_gain;
    Reference::To< MEAL::Scalar > diff_phase;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! Update the transformation with the current estimate, if possible
    void update ();

    //! Update one of the variable parameters
    void update (MEAL::Scalar* function, double value);

  };
}

#endif

