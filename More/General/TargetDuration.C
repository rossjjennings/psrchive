/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TargetDuration.h"
#include "Pulsar/Integration.h"

//! Initialize ranges for the specified parameters
void Pulsar::TimeIntegrate::TargetDuration::initialize (Integrate*,
							Archive* arch)
{
  if (duration <= 0.0)
    throw Error (InvalidState, "TimeIntegrate::TargetDuration::initialize",
                 "duration=%lf", duration);

  ranges.clear ();

  unsigned nsubint = arch->get_nsubint();

  if (nsubint == 0)
    throw Error (InvalidState, "TimeIntegrate::TargetDuration::initialize",
                 "no sub-integrations");

  double current_duration = arch->get_Integration(0)->get_duration();
  unsigned istart = 0;
  unsigned iend = 1;

  while (iend < nsubint)
  {
    double next_duration = arch->get_Integration(iend)->get_duration();

    if (current_duration + next_duration <= duration)
    {
      current_duration += next_duration;
    }
    else
    {
      // end of integration - add a range
      ranges.push_back( std::pair<unsigned,unsigned> (istart, iend) );
      istart = iend;
      current_duration = next_duration;
    }

    iend ++;
  }

  // end of file - add the last range
  ranges.push_back( std::pair<unsigned,unsigned> (istart, iend) );
}

