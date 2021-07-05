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
#include "Pulsar/MeanPolar.h"
#include "MJD.h"

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

    //! The best estimate of the polar model
    MeanPolar polar_estimate;

    //! The index of the SignalPath in which this backend is a component
    unsigned path_index;

    //! The start of the period spanned by this backend solution
    MJD start_time;

    //! The end of the period spanned by this solution
    MJD end_time;

    //! Integrate a calibrator solution
    void integrate (const MEAL::Complex2* xform);

    //! Update the transformation with the current estimate, if possible
    virtual void update ();

    //! Return the path index
    unsigned get_path_index() const { return path_index; }
  };
}

#endif

