#include "Pulsar/Archive.h"
#include "Error.h"

#include <stdlib.h>
#include <unistd.h>

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
  
  if (verbose)
    cerr << "Pulsar::Archive::unload (" << unload_to_filename << ")" << endl;

  // The following section is probably not necessary as consisnency checks are
  // performed when an ephemeris is installed using Archive::set_ephemeris(). 
  // It is included as a backup, should something unexpected happen.
  
  if (ephemeris) {
    if (get_source() != ephemeris->psrname()) {
      cout << "Archive::unload Informative Notice:\n" 
	   << "   Source name will be updated to match archive ephemeris"
	   << endl;
      set_source(ephemeris->psrname());
    }
    if (get_dispersion_measure() != ephemeris->get_dm()) {
      cout << "Archive::unload Informative Notice:\n" 
	   << "   Dispersion measure will be updated to match archive ephemeris"
	   << endl;
      set_dispersion_measure(ephemeris->get_dm());
    }
  }

  // create the temporary filename
  string temp_filename = unload_to_filename + ".XXXXXXXX";

  int fd = mkstemp (const_cast<char*> (temp_filename.c_str()));
  if (fd < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed mkstemp");
  close (fd);

  if (verbose)
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

  unload_filename = unload_to_filename;
  __load_filename = unload_to_filename;
}
