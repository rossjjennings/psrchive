//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/InterpolatedCalibrator.h

#ifndef __InterpolatedCalibrator_H
#define __InterpolatedCalibrator_H

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/CalibrationInterpolatorExtension.h"

namespace Pulsar {

  class CalibratorStokes;

  //! A calibration solution that spans a finite bandwidth and time
  class InterpolatedCalibrator : public PolnCalibrator {
    
  public:

    //! Construct from a solution with a CalibrationInterpolatorExtension
    InterpolatedCalibrator (Archive* data);

    //! Destructor
    ~InterpolatedCalibrator ();

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

  protected:

    //! Fill the transformation vector of the PolnCalibrator base class
    void calculate_transformation ();

    //! Return the number of channels in the PolnCalibrator
    unsigned get_maximum_nchan () const;

    //! Calibration Interpolator Extension
    Reference::To<const CalibrationInterpolatorExtension> interpolator;
    
    //! The Stokes parameters of the input reference signal
    Reference::To<CalibratorStokes> calpoln;

    //! The model of the feed
    Reference::To<PolnCalibratorExtension> feedpar;
  };

}

#endif
