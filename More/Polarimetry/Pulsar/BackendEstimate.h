//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BackendEstimate.h

#ifndef __Pulsar_BackendEstimate_H
#define __Pulsar_BackendEstimate_H

#include "Pulsar/MeanSingleAxis.h"

namespace Calibration
{
  //! Manages a single backend and its current best estimate (first guess)
  class BackendEstimate : public Reference::Able
  {
  public:

    //! The backend transformation
    Reference::To< MEAL::Complex2 > backend;

    //! The best estimate of the backend transformation
    MeanSingleAxis estimate;

    //! The signal path in which this backend is a component
    unsigned path_index;

    //! Update the transformation with the current estimate, if possible
    void update ();
  };
}

#endif
