//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanInstrument.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Calibration_MeanInstrument_H
#define __Calibration_MeanInstrument_H

#include "Calibration/MeanSingleAxis.h"
#include "Calibration/MeanFeed.h"

namespace Calibration {

  class MeanSingleAxis;
  class MeanFeed;

  //! A weighted mean of Instrument parameter estimates
  class MeanInstrument : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current value of the mean
    void update (MEAL::Complex2* model);

  protected:

    //! The mean values of the SingleAxis model parameters
    MeanSingleAxis single_axis;

    //! The mean values of the Feed model parameters
    MeanFeed feed;

  };

}

#endif
