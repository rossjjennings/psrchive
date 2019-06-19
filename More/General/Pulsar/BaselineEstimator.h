//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/BaselineEstimator.h

#ifndef __Pulsar_BaselineFunction_h
#define __Pulsar_BaselineFunction_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar {

  //! ProfileWeight algorithms that compute profile baselines
  class BaselineEstimator : public ProfileWeightFunction {

  public:

    //! Default constructor
    BaselineEstimator ();

    //! Returns a new PhaseWeight instance
    PhaseWeight* baseline (const Profile*);

    //! Returns a new PhaseWeight instance
    PhaseWeight* operator () (const Profile*);

    //! Convenience interface
    PhaseWeight* operate (const Profile*);

    //! Cut samples from baseline with median difference over threshold
    void set_median_cut (float threshold);

    //! Return a copy constructed instance of self
    virtual BaselineEstimator* clone () const = 0;

  protected:

    float median_cut;

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
