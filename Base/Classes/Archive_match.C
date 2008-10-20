/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveMatch.h"

using namespace std;

template<typename F, typename M>
void setup (F& functor, M method)
{
  if (!functor)
    functor = Pulsar::ArchiveMatch::functor (method);
}

bool apply (Pulsar::Archive::MatchFunctor& functor, 
            const Pulsar::Archive* a1, const Pulsar::Archive* a2,
            string& reason)
{
  Pulsar::Archive::MatchResult match = functor (a1, a2);
  reason = match.second;
  return match.first;
}

bool Pulsar::Archive::mixable (const Archive* archive, string& reason) const
{
  setup (mixable_strategy, &ArchiveMatch::set_check_mixable);
  return apply (mixable_strategy, this, archive, reason);
}

bool Pulsar::Archive::standard_match (const Archive* archive,
				      string& reason) const
{
  setup (standard_match_strategy, &ArchiveMatch::set_check_standard);
  return apply (standard_match_strategy, this, archive, reason);
}

bool Pulsar::Archive::processing_match (const Archive* archive,
					string& reason) const
{
  setup (processing_match_strategy, &ArchiveMatch::set_check_processing);
  return apply (processing_match_strategy, this, archive, reason);
}


bool Pulsar::Archive::calibrator_match (const Archive* archive,
					string& reason) const
{
  setup (calibrator_match_strategy, &ArchiveMatch::set_check_calibrator);
  return apply (calibrator_match_strategy, this, archive, reason);
}

