//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelWeight.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/17 13:34:50 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelWeight_H
#define _Pulsar_ChannelWeight_H

#include "Pulsar/Algorithm.h"

namespace Pulsar {
  
  class Archive;
  class Integration;

  //! Algorithms that set the weights of frequency channels
  /*! This pure virtual base class defines the interface to various
      channel weighting algorithms, such as RFI excision. */
  class ChannelWeight : public Algorithm {
    
  public:

    //! Set the weight of each frequency channel in each sub-integration
    virtual void weight (Archive*);

    //! Set the weight of each frequency channel in the sub-integration
    virtual void weight (Integration*) = 0;

  };
  
}

#endif

