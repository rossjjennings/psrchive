//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CalibrationInterpolator.h

#ifndef __CalibrationInterpolator_H
#define __CalibrationInterpolator_H

#include "Pulsar/PolnCalibrator.h"
#include <map>

namespace Pulsar {

  class CalibrationInterpolatorExtension;
  class PolnCalibrator;
  class PolnCalibratorExtension;
  class CalibratorStokes;
  class Integration;
  class SplineSmooth2D;
  
  //! A calibration solution that spans a finite bandwidth and time
  class CalibrationInterpolator : public PolnCalibrator::Variation
  {
    
  public:

    //! Construct from a calibrator with a CalibrationInterpolatorExtension
    CalibrationInterpolator (PolnCalibrator*);

    //! Destructor
    ~CalibrationInterpolator ();

    //! Update the model parameters to match the integration
    /*! Returns true if transformation should be recomputed */
    bool update (const Integration*);
    
  protected:

    //! Calibration Interpolator Extension
    Reference::To<const CalibrationInterpolatorExtension> interpolator;
    
    //! The Stokes parameters of the input reference signal
    Reference::To<CalibratorStokes> calpoln;

    //! The model of the feed
    Reference::To<PolnCalibratorExtension> feedpar;

    std::map< unsigned, Reference::To<SplineSmooth2D> > feedpar_splines;
    std::map< unsigned, Reference::To<SplineSmooth2D> > calpoln_splines;

    MJD last_computed;
  };

}

#endif
