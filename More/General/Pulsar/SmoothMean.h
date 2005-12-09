//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothMean.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_SmoothMean_h
#define __Pulsar_SmoothMean_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile using the mean over a boxcar
  class SmoothMean : public Smooth {

  protected:

    //! The smoothing function
    void smooth_data (unsigned nbin, float* output,
		      unsigned wbin, float* input);

  };

}

#endif
