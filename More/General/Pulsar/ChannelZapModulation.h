//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelZapModulation.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/20 07:37:13 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelZapModulation_H
#define _Pulsar_ChannelZapModulation_H

#include "Pulsar/ChannelWeight.h"

namespace Pulsar {
  
  //! Weights each frequency channel using normalized r.m.s. total intensity
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
