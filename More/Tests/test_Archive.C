#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Reference.h"

int main (int argc, char** argv)
{ try {
  // trap C-style errors and throw Error exception
  Pulsar::Error::handle_signals ();

  // Pulsar::Archive::verbose = true;
  // Pulsar::Error::verbose = true;
  // Pulsar::Integration::verbose = true;

  string filename ("test.ar");

  if (argc>1)
    filename = argv[1];
  
  cerr << "load archive from " << filename << endl;
  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load (filename);

  cerr << "clone archive for tscrunch" << endl;
  Reference::To<Pulsar::Archive> clone = archive -> clone();

  cerr << "tscrunch" << endl;
  clone -> tscrunch ();

  cerr << "tscrunch from " << archive->get_nsubint() 
       << " to "  << clone->get_nsubint() << " sub-integrations" << endl;

  // this will test the Archive destructor
  cerr << "clone archive for fscrunch (destroy tscrunched clone)" << endl;
  clone = archive -> clone();

  cerr << "fscrunch" << endl;
  clone -> fscrunch ();

  cerr << "fscrunch from " << archive->get_nchan() 
       << " to "  << clone->get_nchan() << " sub-channels" << endl;

  cerr << "tscrunch" << endl; 
  clone -> tscrunch ();

  cerr << "unload out.ar" << endl;
  clone -> unload ("out.ar");

}
catch (Pulsar::Error& error) {
  cerr << error << endl;
  return -1;
}
catch (string& error) {
  cerr << error << endl;
  return -1;
}
 
 return 0;
}
