//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ExponentialBaseline.h,v $
   $Revision: 1.1 $
   $Date: 2005/08/16 23:52:06 $
   $Author: straten $ */

#ifndef __Pulsar_ExponentialBaseline_h
#define __Pulsar_ExponentialBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise
  class ExponentialBaseline : public IterativeBaseline {

  protected:

    void get_bounds (PhaseWeight& weight, float& lower, float& upper);

  };

}

#endif
