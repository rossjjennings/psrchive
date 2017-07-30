//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PolnProfileStatsInterface.h

#ifndef __Pulsar_PolnProfileStatsTI_h
#define __Pulsar_PolnProfileStatsTI_h

#include "Pulsar/PolnProfileStats.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class PolnProfileStats::Interface 
    : public TextInterface::To<PolnProfileStats>
  {
  public:

    //! Default constructor
    Interface ( PolnProfileStats* = 0 );

  };

}


#endif
