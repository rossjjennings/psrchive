//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/IterativeBaseline.h,v $
   $Revision: 1.3 $
   $Date: 2005/08/20 14:29:09 $
   $Author: straten $ */

#ifndef __Pulsar_IterativeBaseline_h
#define __Pulsar_IterativeBaseline_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise

  class IterativeBaseline : public BaselineEstimator {

  public:

    //! Default constructor
    IterativeBaseline ();

    //! Destructor
    ~IterativeBaseline ();

    //! Retrieve the PhaseWeight
    void get_weight (PhaseWeight& weight);

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

    //! Derived classes must define the bounds
    virtual void get_bounds (PhaseWeight&, float& lower, float& upper) = 0;

    //! The threshold below which samples are included in the baseline
    float threshold;

    //! The maximum number of iterations
    unsigned max_iterations;

    //! The BaselineEstimator used to find the initial baseline
    Reference::To<BaselineEstimator> initial_baseline;

    //! The Profile from which the mask will be calculated
    Reference::To<const Profile> profile;

  };

}

#endif
