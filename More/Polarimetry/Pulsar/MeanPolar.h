//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanPolar.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 20:45:59 $
   $Author: straten $ */

#ifndef __Calibration_MeanPolar_H
#define __Calibration_MeanPolar_H

#include "MEAL/Mean.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  //! Implements a running mean of the Polar model parameters
  class MeanPolar : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current value of the mean
    void update (MEAL::Complex2* model);

  protected:

    //! Best estimate of receiver gain
    MeanEstimate<double> mean_gain;

    //! Best estimate of receiver boost
    MeanEstimate<double> mean_boostGibbs[3];

    //! Best estimate of receiver rotations
    MeanRadian<double> mean_rotationEuler[3];

  };

}

#endif

