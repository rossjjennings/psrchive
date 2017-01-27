//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/StatisticsInterface.h

#ifndef __Pulsar_StatisticsInterface_h
#define __Pulsar_StatisticsInterface_h

#include "Pulsar/Statistics.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class Statistics::Interface : public TextInterface::To<Statistics>
  {
  public:

    //! Default constructor
    Interface ( Statistics* = 0 );

  };

}


#endif
