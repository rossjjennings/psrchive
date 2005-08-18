//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/IterativeBaseline.h,v $
   $Revision: 1.2 $
   $Date: 2005/08/18 12:09:26 $
   $Author: straten $ */

#ifndef __Pulsar_IterativeBaseline_h
#define __Pulsar_IterativeBaseline_h

#include "Pulsar/BaselineFunction.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise

  class IterativeBaseline : public BaselineFunction {

  public:

    //! Default constructor
    IterativeBaseline ();

    //! Retrieve the PhaseWeight
    void get_weight (PhaseWeight& weight);

    //! Set the Profile from which baseline PhaseWeight will be computed
    void set_Profile (const Profile* profile);

    //! Set the BaselineFunction used to find the initial baseline
    void set_initial_baseline (BaselineFunction*);
    BaselineFunction* get_initial_baseline () const;

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

    //! The BaselineFunction used to find the initial baseline
    Reference::To<BaselineFunction> initial_baseline;

    //! The Profile from which the mask will be calculated
    Reference::To<const Profile> profile;

  };

}

#endif
