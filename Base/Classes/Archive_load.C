#include "Archive.h"
#include "Error.h"

/* Dynamic constructor returns a pointer to a new instance of one of the
   Archive derived classes.   Derived classes must be registered using
   an Archive::Agent.

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

  for (unsigned agent=0; agent<Agent::registry.size(); agent++) try {

    if (Agent::registry[agent]->advocate (filename)) {

      archive = Agent::registry[agent]->new_Archive();

      archive -> load_header (filename);
      archive -> set_filename (filename);
      archive -> __load_filename = filename;

      return archive.release();

    }

  }
  catch (Error& error) {
    throw error += "Pulsar::Archive::load";
  }
  catch (string& error) {
    throw Error (FailedCall, "Pulsar::Archive::load", error);
  }
  
  // none of the registered agents advocates the use of a derived
  // class for the interpretation of this file
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


