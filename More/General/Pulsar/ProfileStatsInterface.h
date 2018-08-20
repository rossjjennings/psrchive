//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileStatsInterface.h

#ifndef __Pulsar_ProfileStatsInterface_h
#define __Pulsar_ProfileStatsInterface_h

#include "Pulsar/ProfileStats.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Profile statistics text interface
  class ProfileStats::Interface : public TextInterface::To<ProfileStats>
  {
  public:

    //! Default constructor
    Interface ( ProfileStats* = 0, bool manage_estimators = true );
  };
}

#endif
