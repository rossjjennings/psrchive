/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ThresholdMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Config.h"

#include "strutil.h"  // for stringprintf

using namespace std;

//! Default constructor
Pulsar::ThresholdMatch::ThresholdMatch ()
{
  maximum_relative_bandwidth = 0;
}

//! Set the maximum relative bandwidth by which archives may differ
void Pulsar::ThresholdMatch::set_maximum_relative_bandwidth (double delta_bw)
{
  maximum_relative_bandwidth = delta_bw;
}

bool Pulsar::ThresholdMatch::get_bandwidth_match (const Archive* a,
                                                  const Archive* b) const
{
  double bwa = a->get_bandwidth();
  double bwb = b->get_bandwidth();
  double bwrel = fabs (bwa - bwb) / fabs (bwa);

  if (bwrel > maximum_relative_bandwidth)
  {
    reason += separator
      + stringprintf ("relative bandwidth mismatch: %lf and %lf", bwa, bwb);
    return false;
  }

  return true;
}


Pulsar::Option<float> bpp_maximum_relative_bandwidth
(
 "BPP::maximum_relative_bandwidth", 5e-3,

 "Tolerable bandwidth difference",

 "The Berkeley Pulsar Processing instruments can have varying bandwidths. \n"
 "Set this parameter to the maximum value of the relative difference \n"
 "that will be tolerated when determining if two archives match each other. \n"
);

//! Return a matching strategy based on the specified method
template <typename T>
static Pulsar::Archive::MatchFunctor bpp_functor (T method)
{
  Pulsar::ThresholdMatch* match = new Pulsar::ThresholdMatch;
  match->set_maximum_relative_bandwidth (bpp_maximum_relative_bandwidth);
  (match->*method) (true);
  return Pulsar::Archive::MatchFunctor (match);
}

void Pulsar::ThresholdMatch::set_BPP (Archive* a)
{
  a->standard_match_strategy 
    = bpp_functor (&ArchiveMatch::set_check_standard);
  a->calibrator_match_strategy 
    = bpp_functor (&ArchiveMatch::set_check_calibrator);
  a->processing_match_strategy 
    = bpp_functor (&ArchiveMatch::set_check_processing);
  a->mixable_strategy 
    = bpp_functor (&ArchiveMatch::set_check_mixable);
}

