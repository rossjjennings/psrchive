
#ifdef PSRFITS
#include "FITSArchive.h"
#endif

#define PSRTIMER 1
#ifdef PSRTIMER
#include "TimerArchive.h"
#include "BasebandArchive.h"
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
  try {

#ifdef PSRFITS
    if (Pulsar::FITSArchive::recognises (filename)) {
      Pulsar::FITSArchive* archive = new Pulsar::FITSArchive;
      archive -> load (filename);
      return archive;
    }
#endif
  
#ifdef PSRTIMER
    if (Pulsar::BasebandArchive::recognises (filename)) {
      Pulsar::BasebandArchive* archive = new Pulsar::BasebandArchive;
      archive -> load (filename);
      return archive;
    }

    if (Pulsar::TimerArchive::recognises (filename)) {
      Pulsar::TimerArchive* archive = new Pulsar::TimerArchive;
      archive -> load (filename);
      return archive;
    }
#endif

  }
  catch (Error& error) {
    throw error += "Archive::load";
  }
  catch (string& error) {
    throw "Archive::load error: " + error;
  }

  // none of the above formats recognises the file

  // check if if can be opened for reading

  FILE* fptr = fopen (filename, "r");

  if (!fptr)
    throw Error (FailedSys, "Archive::load",
		 "cannot open '%s'", filename);

  // it can be opened, but it was not recognized
  fclose (fptr);

  throw Error (InvalidParam, "Archive::load", 
	       "'%s' not a recognized file format", filename);
}
