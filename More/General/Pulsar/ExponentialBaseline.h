//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ExponentialBaseline.h,v $
   $Revision: 1.2 $
   $Date: 2005/09/04 18:20:04 $
   $Author: straten $ */

#ifndef __Pulsar_ExponentialBaseline_h
#define __Pulsar_ExponentialBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise
  class ExponentialBaseline : public IterativeBaseline {

  public:

    //! Default constructor
    ExponentialBaseline ();

  protected:

    void get_bounds (PhaseWeight& weight, float& lower, float& upper);

  };

}

#endif
