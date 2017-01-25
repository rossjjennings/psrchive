//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PolnStatistics.h

#ifndef __Pulsar_PolnStatistics_h
#define __Pulsar_PolnStatistics_h

#include "Pulsar/Statistics.h"

namespace Pulsar
{
  class PolnProfile;
  class PolnProfileStats;

  //! Interface to polarization statistics
  class PolnStatistics : public Statistics::Plugin
  {
  public:

    PolnStatistics ();
    ~PolnStatistics ();

    //! Synchronize to the current state of the Statistic class
    void setup ();

    //! Return a text interface
    TextInterface::Parser* get_interface ();

    //! Return pointer to polarization profile statistics instance
    PolnProfileStats* get_stats ();

  protected:

    mutable Reference::To<PolnProfileStats> stats;
    mutable Reference::To<const PolnProfile> profile;
  };
}

#endif
