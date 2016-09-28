/***************************************************************************
 *
 *   Copyright (C) 2010-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Editor.h"
#include "Pulsar/ArchiveExtension.h"

using namespace std;

void Pulsar::Editor::add_extension (Archive* archive, const std::string& ext)
{
  if (Archive::verbose > 1)
    cerr << "Pulsar::Editor::add_extension " << ext << endl;

  archive->add_extension( Archive::Extension::factory( ext ) );
}

void Pulsar::Editor::remove_extension (Archive* archive, const std::string& ext)
{
  if (Archive::verbose > 1)
    cerr << "Pulsar::Editor::remove_extension " << ext << endl;

  unsigned next = archive->get_nextension();

  for (unsigned i=0; i<next; i++)
    if (archive->get_extension(i)->get_extension_name() == ext
	|| archive->get_extension(i)->get_short_name() == ext)
      delete archive->get_extension(i);
}

//! Get the identifier for this instance
string Pulsar::Editor::get_identifier (const Archive* archive)
{
  return archive->get_filename();
}


