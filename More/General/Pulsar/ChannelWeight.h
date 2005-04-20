//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelWeight.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/20 07:37:13 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelWeight_H
#define _Pulsar_ChannelWeight_H

#include "ReferenceAble.h"

namespace Pulsar {
  
  class Archive;
  class Integration;

  //! Pure virtual baseclass of channel weighting algorithms
  class ChannelWeight : public Reference::Able {
    
  public:

    //! Set the weight of each frequency channel in each sub-integration
    virtual void weight (Archive*);

    //! Set the weight of each frequency channel in the sub-integration
    virtual void weight (Integration*) = 0;

  };
  
}

#endif

