//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/TargetDuration.h

#ifndef __Pulsar_TargetDuration_h
#define __Pulsar_TargetDuration_h

#include "Pulsar/TimeIntegrate.h"
#include "debug.h"

namespace Pulsar {

  class TimeIntegrate::TargetDuration : 
    public Integrate<Archive>::RangePolicy
  {

  public:

    TargetDuration (double seconds = 0.0) { duration = seconds; }

    void set_duration (double seconds) { duration = seconds; }
    double get_duration () const { return duration; }
    
    //! Initialize ranges for the specified parameters
    void initialize (Integrate<Archive>*, Archive*);

    //! Return the number of index ranges into which the container is divided
    unsigned get_nrange () { return ranges.size(); }

    void get_range (unsigned irange, unsigned& start, unsigned& stop)
    {
      start = ranges[irange].first;
      stop = ranges[irange].second;
    }

  protected:

    //! The start and end input subint indexes for each output subint
    std::vector< std::pair<unsigned,unsigned> > ranges;

    //! Output sub-integration length in seconds
    double duration;
    
  };

}

#endif
