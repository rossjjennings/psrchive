//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineEstimator.h,v $
   $Revision: 1.8 $
   $Date: 2007/11/02 04:24:55 $
   $Author: straten $ */

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

    //! Include only the specified phase bins for consideration
    void set_include (PhaseWeight* include);

    //! Cut samples from baseline with median difference over threshold
    void set_median_cut (float threshold);

  protected:

    //! Excluded phase bins
    Reference::To<PhaseWeight> include;

    float median_cut;

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
