/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Contemporaneity.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace Pulsar;

double double_cast (const MJD& mjd)
{
  return mjd.in_seconds();
}

double double_cast (const Phase& phase)
{
  return phase.in_turns();
}

// returns the fractional time span over which events A and B overlap
template<typename T>
double get_overlap_ratio (const T& startA, const T& endA,
			  const T& startB, const T& endB)
{
  T overlap_start = std::max (startA, startB);
  T overlap_end = std::min (endA, endB);

  if (overlap_end < overlap_start)
    return 0;

  T total_start = std::min (startA, startB);
  T total_end = std::max (endA, endB);

  double overlap_length = double_cast( overlap_end - overlap_start );
  double total_length = double_cast( total_end - total_start );

  return overlap_length / total_length;
}

void 
Contemporaneity::AtPulsar::set_archives (const Archive* A, const Archive* B)
{
  predA = A->get_model ();
  predB = B->get_model ();
}

//! Return a fraction between 0 (no overlap) and 1 (complete overlap) 
double Contemporaneity::AtPulsar::evaluate (const Integration* A,
					    const Integration* B)
{
  Phase startA = predA->phase( A->get_epoch() );
  Phase endA = startA + 1.0;

  Phase startB = predB->phase( B->get_epoch() );
  Phase endB = startB + 1.0;

  return get_overlap_ratio (startA, endA, startB, endB);
}



//! Retrieve any additional information that may be required
void Contemporaneity::AtEarth::set_archives (const Archive*, const Archive*)
{
}


// returns the start and end time of a sub-integration
void get_start_end (const Integration* subint, MJD& start, MJD& end)
{
  MJD epoch = subint->get_epoch();
  double length = subint->get_duration();
  double period = subint->get_folding_period();

  if (length < 1.1 * period)
  {
    /* most likely a single-pulse integration, in which case the rise
       time of the first phase bin will equal the start time */

    start = end = epoch;
    end += period;
  }
  else
  {
    /* most likely a longer integration, in which case the rise time
       of the first phase bin will equal the mid time */

    start = end = epoch;
    start -= 0.5 * length;
    end += 0.5 * length;
  }
}

//! Return a fraction between 0 (no overlap) and 1 (complete overlap) 
double Contemporaneity::AtEarth::evaluate (const Integration* A,
					   const Integration* B)
{
  MJD startA, endA;
  get_start_end (A, startA, endA);
  
  MJD startB, endB;
  get_start_end (B, startB, endB);

  return get_overlap_ratio (startA, endA, startB, endB);
}
