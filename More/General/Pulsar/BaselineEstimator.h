//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineFunction_h
#define __Pulsar_BaselineFunction_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! ProfileWeight algorithms that compute profile baselines
  class BaselineEstimator : public ProfileWeightFunction {

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
