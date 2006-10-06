//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/OnPulseEstimator.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
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
