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
void setup (F& policy, M method)
{
  if (!policy)
    policy = Pulsar::Archive::Match::factory (method);
}

bool apply (Pulsar::Archive::Match* policy, 
            const Pulsar::Archive* a1, const Pulsar::Archive* a2,
            string& reason)
{
  bool result = policy->match (a1, a2);
  reason = policy->get_reason ();
  return result;
}

bool Pulsar::Archive::mixable (const Archive* archive, string& reason) const
{
  setup (mixable_policy, &Archive::Match::set_check_mixable);
  return apply (mixable_policy, this, archive, reason);
}

bool Pulsar::Archive::standard_match (const Archive* archive,
				      string& reason) const
{
  setup (standard_match_policy, &Archive::Match::set_check_standard);
  return apply (standard_match_policy, this, archive, reason);
}

bool Pulsar::Archive::processing_match (const Archive* archive,
					string& reason) const
{
  setup (processing_match_policy, &Archive::Match::set_check_processing);
  return apply (processing_match_policy, this, archive, reason);
}


bool Pulsar::Archive::calibrator_match (const Archive* archive,
					string& reason) const
{
  setup (calibrator_match_policy, &Archive::Match::set_check_calibrator);
  return apply (calibrator_match_policy, this, archive, reason);
}

const Pulsar::Archive::Match* Pulsar::Archive::get_standard_match () const
{
  return standard_match_policy;
}

void Pulsar::Archive::set_standard_match (Match* policy)
{
  standard_match_policy = policy;
}

//! Policy determines if a calibrator matches an observation
const Pulsar::Archive::Match* Pulsar::Archive::get_calibrator_match () const
{
  return calibrator_match_policy;
}

void Pulsar::Archive::set_calibrator_match (Match* policy)
{
  calibrator_match_policy = policy;
}

//! Policy determines if data were processed identically
const Pulsar::Archive::Match* Pulsar::Archive::get_processing_match () const
{
  return processing_match_policy;
}

void Pulsar::Archive::set_processing_match (Match* policy)
{
  processing_match_policy = policy;
}

//! Policy determines if data can be combined/integrated
const Pulsar::Archive::Match* Pulsar::Archive::get_mixable () const
{
  return mixable_policy;
}

void Pulsar::Archive::set_mixable (Match* policy)
{
  mixable_policy = policy;
}
