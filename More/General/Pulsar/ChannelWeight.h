//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ChannelWeight.h

#ifndef _Pulsar_ChannelWeight_H
#define _Pulsar_ChannelWeight_H

#include "Pulsar/Weight.h"

namespace Pulsar {
  
  //! Algorithms that set the weights of frequency channels
  /*! This base class defines the interface to various
      channel weighting algorithms, such as RFI excision. */
  class ChannelWeight : public Weight {

  };
  
}

#endif

