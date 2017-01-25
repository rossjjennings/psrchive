//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/RFIMitigation/Pulsar/RobustMower.h

#ifndef __Pulsar_RobustMower_h
#define __Pulsar_RobustMower_h

#include "Pulsar/Mower.h"

namespace Pulsar {

  //! Lawn mower algorithm uses robust statistics to find spikes
  class RobustMower : public Mower
  {

  protected:
    
    //! Find the spikes in median smoothed difference and flag them in mask
    void compute (PhaseWeight* mask, const Profile* difference);

  };

}

#endif
