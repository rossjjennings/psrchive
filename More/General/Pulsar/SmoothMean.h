//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothMean.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/15 15:44:07 $
   $Author: straten $ */

#ifndef __Pulsar_SmoothMean_h
#define __Pulsar_SmoothMean_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a pulse profile using the mean over a boxcar
  class SmoothMean : public Smooth {

  protected:

    //! The smoothing function
    void smooth_data (unsigned nbin, float* output,
		      unsigned wbin, float* input);

  };

}

#endif
