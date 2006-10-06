//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeightFunction.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeightFunction_h
#define __Pulsar_PhaseWeightFunction_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  class PhaseWeight;

  //! Algorithms that produce PhaseWeight objects
  /*! This pure virtual base class of PhaseWeight algorithms defines the
      interface by which various routines, such as baseline estimators
      can be accessed */
  class PhaseWeightFunction : public Algorithm {

  public:

    //! Retrieve the PhaseWeight
    virtual void get_weight (PhaseWeight& weight) = 0;

  };

}


#endif // !defined __Pulsar_PhaseWeightFunction_h
