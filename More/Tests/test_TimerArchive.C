#include <cpgplot.h>

#include "TimerArchive.h"
#include "Integration.h"
#include "Profile.h"
#include "Error.h"

int main (int argc, char** argv)  {

  try {

    Pulsar::Archive::verbose = true;
    Pulsar::Integration::verbose = true;
   
    Pulsar::TimerArchive archive;
    
    if (argc > 1)
      archive.load (argv[1]);
    
    archive.pscrunch();
    archive.fscrunch();
    archive.tscrunch();

    cpgbeg (0, "?", 0, 0);
    archive.get_Profile(0,0,0)->display();
    cpgend ();

  }

  catch (Pulsar::Error& error) {
    cerr << error << endl;
    return -1;
  }

  return 0;
}

