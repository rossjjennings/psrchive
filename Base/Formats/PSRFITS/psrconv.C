#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "Pulsar/Archive.h"
#include "Pulsar/FITSArchive.h"
#include "Error.h"

void usage() {

  cout << "A simple program to convert pulsar archives to FITS format" << endl;
  cout << "Usage: psrconv (options) <filename>" << endl;
  cout << "Where the options are as follows:" << endl;
  cout << "     -h     This help message" << endl;
  cout << "     -v     Verbose mode" << endl;
  cout << "     -V     Very verbose mode" << endl;

}

int main(int argc, char *argv[]) {

  bool verbose = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hVva")) != -1) {
    switch (gotc) {
    case 'h':
      usage();
      return(0);
      break;
    case 'V':
      Pulsar::FITSArchive::verbose = true;
      verbose = true;
      break;
    case 'v':
      verbose = true;
      break;
    case 'a':
      Pulsar::Archive::Agent::report ();
      return 0;
    }
  }
  
  Reference::To<Pulsar::Archive> arch;
  Pulsar::FITSArchive* fitsarch = 0;

  if ((argc - optind )!= 1) {
    usage();
    exit(-1);
  }

  try {
    
    arch = Pulsar::Archive::load(argv[optind]);
    cerr << "Loaded. " << argv[optind] << endl;
    
    fitsarch = new Pulsar::FITSArchive(*arch);
    cerr << "Created FITS archive." << endl;
    
    cerr << "FITS Archive stats:" << endl;
    cerr << "Source: " << fitsarch -> get_source() << endl;
    cerr << "Frequency: " << fitsarch -> get_centre_frequency() << endl;
    cerr << "Bandwidth: " << fitsarch -> get_bandwidth() << endl;
    cerr << "# of subints: " << fitsarch -> get_nsubint() << endl;
    cerr << "# of polns: " << fitsarch -> get_npol() << endl;
    cerr << "# of channels: " << fitsarch -> get_nchan() << endl;
    cerr << "# of bins: " << fitsarch -> get_nbin() << endl;

    fitsarch -> unload("data.fits");
    cerr << "Unloaded FITS Archive." << endl;
  }
  catch (Error& error) {
    cerr << error << endl;
  }
  
}


