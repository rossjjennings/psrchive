//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.7 $
   $Date: 2007/11/01 04:05:50 $
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

    //! Include only the specified phase bins for consideration
    void set_include (PhaseWeight* include);

  protected:

    //! Excluded phase bins
    Reference::To<PhaseWeight> include;

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
