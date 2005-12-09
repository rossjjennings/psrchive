//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineFunction_h
#define __Pulsar_BaselineFunction_h

#include "Pulsar/PhaseWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! PhaseWeight algorithms that compute profile baselines
  /*! This pure virtual base class defines the interface through which
      various baseline finding algorithms are accessed */
  class BaselineEstimator : public PhaseWeightFunction {

  public:

    //! Set the Profile from which the baseline PhaseWeight will be derived
    virtual void set_Profile (const Profile* profile) = 0;

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
