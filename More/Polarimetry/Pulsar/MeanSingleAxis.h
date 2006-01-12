//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanSingleAxis.h,v $
   $Revision: 1.3 $
   $Date: 2006/01/12 17:30:34 $
   $Author: straten $ */

#ifndef __Calibration_MeanSingleAxis_H
#define __Calibration_MeanSingleAxis_H

#include "MEAL/Mean.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  class SingleAxis;

  //! A weighted mean of SingleAxis parameter estimates
  class MeanSingleAxis : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current mean
    void update (MEAL::Complex2* model) const;

    //! Add the SingleAxis model parameters to the running mean
    void integrate (const SingleAxis* model);

    //! Update the SingleAxis model parameters with the current mean
    void update (SingleAxis* model) const;

  protected:

    //! Best estimate of instrumental gain
    MeanEstimate<double> mean_gain;

    //! Best estimate of differential gain
    MeanEstimate<double> mean_diff_gain;

    //! Best estimate of differential phase
    MeanRadian<double> mean_diff_phase;

  };

}

#endif

