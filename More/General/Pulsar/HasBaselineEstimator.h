//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/HasBaselineEstimator.h

#ifndef __Pulsar_HasBaselineEstimator_h
#define __Pulsar_HasBaselineEstimator_h

#include "ProfileWeightFunction.h"

namespace Pulsar
{
  class Profile;
  class ProfileWeightFunction;

  //! Manages a baseline estimator and its interface
  class HasBaselineEstimator
  {   
  public:

    //! Default constructor
    HasBaselineEstimator ();

    //! Copy constructor
    HasBaselineEstimator (const HasBaselineEstimator&);

    //! Destructor
    ~HasBaselineEstimator();

    //! Set the algorithm used to find the off-pulse phase bins
    virtual void set_baseline_estimator (ProfileWeightFunction*);

    //! Get the algorithm used to find the off-pulse phase bins
    ProfileWeightFunction* get_baseline_estimator () const;

    //! Share the baseline estimator of other
    void share (HasBaselineEstimator* other);

    template<typename Child>
    static void import (typename Child::Interface*);

  private:

    //! The algorithm used to find the off-pulse phase bins
    Reference::To<ProfileWeightFunction> baseline_estimator;

    //! Share from another object that has a baseline estimator
    HasBaselineEstimator* other;
  };

  template<typename Child>
  void HasBaselineEstimator::import (typename Child::Interface* interface)
  {
    interface->add( &HasBaselineEstimator::get_baseline_estimator,
		    &HasBaselineEstimator::set_baseline_estimator,
		    &ProfileWeightFunction::get_interface,
		    "off", "Off-pulse estimator" );
  };

}


#endif



