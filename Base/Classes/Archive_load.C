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
Pulsar::Archive* Pulsar::Archive::load (const string& filename)
{
  // check if file can be opened for reading
  FILE* fptr = fopen (filename.c_str(), "r");

  if (!fptr) throw Error (FailedSys, "Pulsar::Archive::load",
			  "cannot open '%s'", filename.c_str());
  fclose (fptr);
  
  if (Agent::registry.size() == 0)
    throw Error (InvalidState, "Pulsar::Archive::load", "no Agents loaded");

  // see if any of the derived classes recognize the file
  Reference::To<Archive> archive;

  if (verbose == 3)
    cerr << "Pulsar::Archive::load with " << Agent::registry.size() 
         << " Agents" << endl;

  for (unsigned agent=0; agent<Agent::registry.size(); agent++) try {

    if (verbose == 3)
      cerr << "Pulsar::Archive::load testing "
           << Agent::registry[agent]->get_name() << endl;

    if (Agent::registry[agent]->advocate (filename.c_str())) {

      if (verbose == 3)
        cerr << "Pulsar::Archive::load using " 
	     << Agent::registry[agent]->get_name() << endl;

      archive = Agent::registry[agent]->new_Archive();
      
      archive -> __load_filename = filename;
      archive -> load_header (filename.c_str());
      archive -> set_filename (filename);

      return archive.release();

    }

  }
  catch (Error& error) {
    if (verbose == 3)
      cerr << "Pulsar::Archive::load error " << error << endl;
    throw error += "Pulsar::Archive::load";
  }
  catch (string& error) {
    throw Error (FailedCall, "Pulsar::Archive::load", error);
  }
  
  // none of the registered agents advocates the use of a derived
  // class for the interpretation of this file
  throw Error (InvalidParam, "Pulsar::Archive::load", 
	       "'%s' not a recognized file format", filename.c_str());
}

