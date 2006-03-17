/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ArchiveSort.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "ModifyRestore.h"

#include <algorithm>

Pulsar::ArchiveSort::ArchiveSort (const Archive* archive)
{
  if (!archive) {
    centre_frequency = 0.0;
    return;
  }

  filename         = archive->get_filename();
  source           = archive->get_source();
  centre_frequency = archive->get_centre_frequency();
  epoch            = archive->get_Integration(0)->get_epoch();
}

bool Pulsar::operator < (const ArchiveSort& a, const ArchiveSort& b)
{
  if (a.source < b.source)
    return true;
  if (a.centre_frequency < b.centre_frequency)
    return true;
  if (a.epoch < b.epoch)
    return true;
  return false;
}

void Pulsar::ArchiveSort::sort (std::vector<std::string>& filenames)
{
  std::vector<ArchiveSort> entries;

  load (filenames, entries);

  filenames.resize (entries.size());
  
  for (unsigned ifile = 0; ifile < filenames.size(); ifile++)
    filenames[ifile] = entries[ifile].filename;
}

void Pulsar::ArchiveSort::load (const std::vector<std::string>& filenames,
				std::vector<ArchiveSort>& entries)
{
  entries.resize (filenames.size());
  unsigned ientry = 0;

  ModifyRestore<bool> mod (Profile::no_amps, true);

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {

    Reference::To<Archive> archive = Archive::load (filenames[ifile]);
    entries[ientry] = ArchiveSort (archive);
    ientry ++;

  }
  catch (Error& error) {
    cerr << "Pulsar::ArchiveSort::sort could not load '" << filenames[ifile]
	 << "' " << error.get_message() << endl;
  }

  entries.resize (ientry);
  std::sort (entries.begin(), entries.end());
}
