//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelWeight.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/23 21:18:46 $
   $Author: straten $ */

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

