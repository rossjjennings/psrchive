//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeightFunction.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/27 15:28:58 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeightFunction_h
#define __Pulsar_PhaseWeightFunction_h

#include "Reference.h"

namespace Pulsar {

  class PhaseWeight;

  //! Pure virtual base class of Phase weight function producers
  class PhaseWeightFunction : public Reference::Able {

  public:

    //! Retrieve the PhaseWeight
    virtual void get_weight (PhaseWeight& weight) = 0;

  };

}


#endif // !defined __Pulsar_PhaseWeightFunction_h
