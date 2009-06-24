//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/GaussianBaseline.h,v $
   $Revision: 1.8 $
   $Date: 2009/06/24 05:02:23 $
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
    float get_threshold () const { return threshold; }

    //! Set the smoothing factor used during post processing
    void set_smoothing (unsigned);

    //! Get the variance correction factor
    float get_variance_correction () const { return moment_correction; }

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    GaussianBaseline* clone () const;

  protected:

    void get_bounds (PhaseWeight* weight, float& lower, float& upper);

    void postprocess (PhaseWeight* weight, const Profile* profile);

    //! Adjustment when computing variance of samples below threshold
    float moment_correction;

    //! Value of lower last returned by get_bounds
    float last_lower;

    //! Value of upper last returned by get_bounds
    float last_upper;

    //! Last mean computed during get_bounds
    float last_mean;

    //! Smoothing factor used in postprocessing
    unsigned smooth_bins;

  };

}

#endif
