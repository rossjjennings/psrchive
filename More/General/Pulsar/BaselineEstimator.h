//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.1 $
   $Date: 2005/08/20 14:29:09 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineFunction_h
#define __Pulsar_BaselineFunction_h

#include "Pulsar/PhaseWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! Pure virtual base class of baseline phase weighting functions
  class BaselineEstimator : public PhaseWeightFunction {

  public:

    //! Set the Profile from which the baseline PhaseWeight will be derived
    virtual void set_Profile (const Profile* profile) = 0;

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
