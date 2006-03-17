/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveMatch.h"

bool Pulsar::Archive::mixable (const Archive* archive, string& reason) const
{
  ArchiveMatch match;
  match.set_check_mixable ();

  bool result = match.match (this, archive);
  reason = match.get_reason ();

  return result;
}


bool Pulsar::Archive::standard_match (const Archive* archive,
				      string& reason) const
{
  ArchiveMatch match;
  match.set_check_standard ();

  bool result = match.match (this, archive);
  reason = match.get_reason ();

  return result;
}


bool Pulsar::Archive::processing_match (const Archive* archive,
					string& reason) const
{
  ArchiveMatch match;
  match.set_check_processing ();

  bool result = match.match (this, archive);
  reason = match.get_reason ();

  return result;
}


bool Pulsar::Archive::calibrator_match (const Archive* archive,
					string& reason) const
{
  ArchiveMatch match;
  match.set_check_calibrator ();

  bool result = match.match (this, archive);
  reason = match.get_reason ();

  return result;
}

