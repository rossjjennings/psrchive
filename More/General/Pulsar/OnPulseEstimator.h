//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/OnPulseEstimator.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/13 07:02:30 $
   $Author: straten $ */

#ifndef __Pulsar_OnPulseEstimator_h
#define __Pulsar_OnPulseEstimator_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! ProfileWeight algorithms that compute on-pulse phase bins
  class OnPulseEstimator : public ProfileWeightFunction {

  };

}


#endif // !defined __Pulsar_OnPulseEstimator_h
