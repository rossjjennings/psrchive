//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/GaussianBaseline.h,v $
   $Revision: 1.4 $
   $Date: 2007/02/12 17:40:58 $
   $Author: straten $ */

#ifndef __Pulsar_GaussianBaseline_h
#define __Pulsar_GaussianBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise
  class GaussianBaseline : public IterativeBaseline {

  public:

    //! Default constructor
    GaussianBaseline ();

    //! Set the threshold below which samples are included in the baseline
    void set_threshold (float sigma);

  protected:

    void get_bounds (PhaseWeight& weight, float& lower, float& upper);

    //! Adjustment when computing variance of samples below threshold
    float moment_correction;

  };

}

#endif
