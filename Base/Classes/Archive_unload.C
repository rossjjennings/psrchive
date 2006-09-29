/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
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
void Pulsar::Archive::unload (const char* filename)
{
  string unload_to_filename = unload_filename;
  if (filename)
    unload_to_filename = filename;

  if (verbose == 3)
    cerr << "Pulsar::Archive::unload (" << unload_to_filename << ")" << endl;

  // create the temporary filename
  string temp_filename = unload_to_filename + ".XXXXXXXX";

  int fd = mkstemp (const_cast<char*> (temp_filename.c_str()));
  if (fd < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed mkstemp");
  close (fd);

  if (verbose == 3)
    cerr << "Pulsar::Archive::unload calling unload_file "
      "(" << temp_filename << ")" << endl;

  try {
    unload_file (temp_filename.c_str());
  }
  catch (Error& error) {
    remove (temp_filename.c_str());
    throw error += "Pulsar::Archive::unload";
  }
  catch (...) {
    remove (temp_filename.c_str());
    throw Error (FailedCall, "Pulsar::Archive::unload", "failed unload_file "
		 "(" + temp_filename + ")");
  }

  // rename the temporary file with the requested filename
  int ret = rename (temp_filename.c_str(), unload_to_filename.c_str());

  if (ret < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed rename");

  ret = chmod (unload_to_filename.c_str(), 0666 & ~getumask());

  if (ret < 0 && verbose)  {
    char temp[8];
    sprintf (temp, "%x", 0666 & ~getumask());
    cerr << "Pulsar::Archive::unload WARNING failed chmod ("
         << unload_to_filename << ", " << temp << ")" << endl;
  }

  unload_filename = unload_to_filename;
  __load_filename = unload_to_filename;
}
