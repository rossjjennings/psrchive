/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "TemporaryFile.h"
#include "Error.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

mode_t getumask(void) {
  mode_t mask = umask(0);
  umask(mask);
  return mask;
}

/*! To protect data, especially when writing the output archive to a
  file of the same name as the input archive, this method unloads data
  to a temporary file before renaming the archive to the requested
  output file name.  The temporary file will be written using a unique
  filename in the same path as the requested output file. */
void Pulsar::Archive::unload (const char* filename) const
{
  // run all verifications
  verify ();

  string unload_to_filename = unload_filename;
  if (filename)
    unload_to_filename = filename;

  if (verbose == 3)
    cerr << "Pulsar::Archive::unload (" << unload_to_filename << ")" << endl;

  TemporaryFile temp (unload_to_filename);

  try {
    unload_file (temp.get_filename().c_str());
  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::unload";
  }

  // rename the temporary file with the requested filename
  int ret = rename (temp.get_filename().c_str(), unload_to_filename.c_str());

  if (ret < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed rename");

  temp.set_removed (true);

  ret = chmod (unload_to_filename.c_str(), 0666 & ~getumask());

  if (ret < 0 && verbose)  {
    char temp[8];
    sprintf (temp, "%x", 0666 & ~getumask());
    cerr << "Pulsar::Archive::unload WARNING failed chmod ("
         << unload_to_filename << ", " << temp << ")" << endl;
  }

  const_cast<Archive*>(this)->unload_filename = unload_to_filename;
  const_cast<Archive*>(this)->__load_filename = unload_to_filename;
}
