//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.6 $
   $Date: 2007/07/30 06:03:28 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineFunction_h
#define __Pulsar_BaselineFunction_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar {

  //! ProfileWeight algorithms that compute profile baselines
  class BaselineEstimator : public ProfileWeightFunction {

  public:

    //! Returns a new PhaseWeight instance
    PhaseWeight* baseline (const Profile*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
