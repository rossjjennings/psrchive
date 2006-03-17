//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothMedian.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/17 13:34:51 $
   $Author: straten $ */

#ifndef __Pulsar_SmoothMedian_h
#define __Pulsar_SmoothMedian_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile using the median over a boxcar
  class SmoothMedian : public Smooth {

  protected:

    //! The smoothing function
    void smooth_data (unsigned nbin, float* output,
		      unsigned wbin, float* input);

  };

}

#endif
