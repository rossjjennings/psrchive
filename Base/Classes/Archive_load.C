
#ifdef PSRFITS
#include "Pulsar/FITSArchive.h"
#endif

#define PSRTIMER 1
#ifdef PSRTIMER
#include "Pulsar/TimerArchive.h"
#include "Pulsar/BasebandArchive.h"
#endif

#include "Error.h"

/* Dynamic constructor returns a pointer to a new instance of one of the
   Archive subclasses.  This is the only member of the Archive
   class that must know about the derived types, and is the entry point
   for loading any new archive from file.  The recognized children must
   be added to the source code manually.

   \param filename path to the file containing a pulsar archive
*/
Pulsar::Archive* Pulsar::Archive::load (const char* filename)
{
  // check if file can be opened for reading
  FILE* fptr = fopen (filename, "r");

  if (!fptr) throw Error (FailedSys, "Pulsar::Archive::load",
			  "cannot open '%s'", filename);

  fclose (fptr);

  // see if any of the derived classes recognize the file
  Reference::To<Archive> archive;

  try {

#ifdef PSRFITS
    if (Pulsar::FITSArchive::recognises (filename))
      archive = new Pulsar::FITSArchive;
    else
#endif
  
#ifdef PSRTIMER
    if (Pulsar::BasebandArchive::recognises (filename))
      archive = new Pulsar::BasebandArchive;
    else

    if (Pulsar::TimerArchive::recognises (filename))
      archive = new Pulsar::TimerArchive;
#endif

    if (archive) {
      // call the pure virtual load_header
      archive -> load_header (filename);
      // set the filename from which data for this instance can be loaded
      archive -> __load_filename = filename;
      archive -> set_filename (filename);
      return archive.release();
    }

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::load";
  }
  catch (string& error) {
    throw Error (FailedCall, "Pulsar::Archive::load", error);
  }

  // none of the above formats recognises the file
  throw Error (InvalidParam, "Pulsar::Archive::load", 
	       "'%s' not a recognized file format", filename);
}

void Pulsar::Archive::refresh()
{
  if (verbose)
    cerr << "Pulsar::Archive::refresh" << endl;

  IntegrationManager::resize(0);

  load_header (__load_filename.c_str());
}

void Pulsar::Archive::update()
{
  if (verbose)
    cerr << "Pulsar::Archive::update" << endl;

  load_header (__load_filename.c_str());
}


Pulsar::Integration* Pulsar::Archive::load_Integration (unsigned isubint)
{
  if (verbose)
    cerr << "Pulsar::Archive::load_Integration" << endl;

  if (!__load_filename.length())
    throw Error (InvalidState, "Pulsar::Archive::load_Integration",
		 "internal error: instance not loaded from file");

  return load_Integration (__load_filename.c_str(), isubint);
}


