//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Weight.h

#ifndef _Pulsar_Weight_H
#define _Pulsar_Weight_H

#include "Pulsar/Algorithm.h"

namespace Pulsar {
  
  class Archive;
  class Integration;

  //! Algorithms that set the weights of all Profiles in an Archive
  class Weight : public Algorithm {
    
  public:

    //! Set the weights of all Profiles in the Archive
    virtual void operator () (Archive*);

  protected:

    //! Set the weight of each frequency channel in the sub-integration
    virtual void weight (Integration*) = 0;

  };
  
}

#endif

