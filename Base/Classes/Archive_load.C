#include "Pulsar/Archive.h"
#include "Error.h"

/* Dynamic constructor returns a pointer to a new instance of one of the
   Archive derived classes.   Derived classes must be registered using
   an Archive::Agent.

   \param filename path to the file containing a pulsar archive

   Note: the definition of this static member function is contained in a 
   source code file separate from all other archive methods so that it will
   be linked into executables only when called directly.
*/
Pulsar::Archive* Pulsar::Archive::load (const char* filename)
{
  // check if file can be opened for reading
  FILE* fptr = fopen (filename, "r");

  if (!fptr) throw Error (FailedSys, "Pulsar::Archive::load",
			  "cannot open '%s'", filename);
  fclose (fptr);
  
  Agent::init ();

  if (Agent::registry.size() == 0)
    throw Error (InvalidState, "Pulsar::Archive::load", "no Agents loaded");

  // see if any of the derived classes recognize the file
  Reference::To<Archive> archive;

  for (unsigned agent=0; agent<Agent::registry.size(); agent++) try {

    if (Agent::registry[agent]->advocate (filename)) {

      if (verbose)
        cerr << "Pulsar::Archive::load using " 
	     << Agent::registry[agent]->get_name() << endl;

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

Pulsar::Archive* Pulsar::Archive::load (const string& filename)
{ 
  return load (filename.c_str());
}

