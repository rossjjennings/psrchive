/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/UnloadOptions.h"
#include "Pulsar/Archive.h"

#include "strutil.h"

Pulsar::UnloadOptions::UnloadOptions ()
{
  overwrite = false;
}

//! Extra usage information implemented by derived classes
std::string Pulsar::UnloadOptions::get_usage ()
{
  return 
    " -m               modify (overwrite) the original file \n"
    " -e ext           write files with a new extension \n"
    " -O path          write files to a new directory \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::UnloadOptions::get_options ()
{
  return "e:mO:";
}

//! Parse a non-unload command
bool Pulsar::UnloadOptions::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'e':
    extension = arg;
    break;
    
  case 'm':
    overwrite = true;
    break;

  case 'O':
    directory = arg;
    break;

  default:
    return false;
  }

  return true;
}

void Pulsar::UnloadOptions::setup ()
{
  cerr << "Pulsar::UnloadOptions::setup" << endl;

  if (overwrite && (!extension.empty() || !directory.empty()))
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "cannot use -m option with -e and/or -O option");

  if (!overwrite && extension.empty() && directory.empty())
    throw Error (InvalidState, "Pulsar::UnloadOptions::setup",
		 "please specify either the -m option or the -e or -O option");
}

//! Unload the archive
void Pulsar::UnloadOptions::finish (Archive* archive)
{
  if (overwrite)
  {
#ifdef _DEBUG
    cerr << "Pulsar::UnloadOptions::finish overwriting "
	 << archive->get_filename() << endl;
#endif

    cout << "Unloading " << archive->get_filename () << " ..." << endl;
    archive->unload ();
    cout << archive->get_filename () << " updated on disk" << endl;
    return;
  }

  string newname = archive->get_filename();

  if (!extension.empty())
    newname = replace_extension( newname, extension );

  if (!directory.empty())
    newname = directory + "/" + basename (newname);

#ifdef _DEBUG
  cerr << "Pulsar::UnloadOptions::finish writing " << newname << endl;
#endif

  cout << "Unloading " << newname << " ..." << endl;
  archive->unload (newname);
  cout << "New file " << newname << " written to disk" << endl;
}
