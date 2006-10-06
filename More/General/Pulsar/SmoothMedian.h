//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothMedian.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
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
