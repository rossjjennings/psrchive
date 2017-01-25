//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ChannelZapModulation.h

#ifndef _Pulsar_ChannelZapModulation_H
#define _Pulsar_ChannelZapModulation_H

#include "Pulsar/ChannelWeight.h"

namespace Pulsar {
  
  //! Weights each channel using normalized r.m.s. total intensity
  class ChannelZapModulation : public ChannelWeight {
    
  public:
    
    //! Default constructor
    ChannelZapModulation ();

    //! Set integration weights
    void weight (Integration* integration);

  protected:

    float cutoff_threshold;

  };
  
}

#endif
