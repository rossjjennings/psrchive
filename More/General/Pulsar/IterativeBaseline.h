//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/IterativeBaseline.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/17 13:34:51 $
   $Author: straten $ */

#ifndef __Pulsar_IterativeBaseline_h
#define __Pulsar_IterativeBaseline_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  //! Finds a Profile baseline using an interative bounding method
  class IterativeBaseline : public BaselineEstimator {

  public:

    //! Default constructor
    IterativeBaseline ();

    //! Destructor
    ~IterativeBaseline ();

    //! Set the Profile from which baseline PhaseWeight will be computed
    void set_Profile (const Profile* profile);

    //! Set the BaselineEstimator used to find the initial baseline
    void set_initial_baseline (BaselineEstimator*);
    BaselineEstimator* get_initial_baseline () const;

    //! Set the threshold below which samples are included in the baseline
    void set_threshold (float sigma);

    //! Set the maximum number of iterations
    void set_max_iterations (unsigned iterations);

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight& weight);

    //! Derived classes must define the bounds
    virtual void get_bounds (PhaseWeight&, float& lower, float& upper) = 0;

    //! The threshold below which samples are included in the baseline
    float threshold;

    //! The maximum number of iterations
    unsigned max_iterations;

    //! The BaselineEstimator used to find the initial baseline
    Reference::To<BaselineEstimator> initial_baseline;

  };

}

#endif
