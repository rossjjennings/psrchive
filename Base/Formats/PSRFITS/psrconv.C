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
    }
  }
  
  Reference::To<Pulsar::Archive> arch;
  Pulsar::FITSArchive* fitsarch = 0;
  
  if ((argc - optind )!= 1) {
    usage();
    exit(-1);
  }

  try {
   
    cerr << "Loading " << argv[optind] << endl;
    arch = Pulsar::Archive::load(argv[optind]);
    
    fitsarch = new Pulsar::FITSArchive(*arch);
    cerr << "Conversion complete" << endl;
    
    if (verbose) {
      cerr << "Source: " << fitsarch -> get_source() << endl;
      cerr << "Frequency: " << fitsarch -> get_centre_frequency() << endl;
      cerr << "Bandwidth: " << fitsarch -> get_bandwidth() << endl;
      cerr << "# of subints: " << fitsarch -> get_nsubint() << endl;
      cerr << "# of polns: " << fitsarch -> get_npol() << endl;
      cerr << "# of channels: " << fitsarch -> get_nchan() << endl;
      cerr << "# of bins: " << fitsarch -> get_nbin() << endl;
    }

    string newname = arch->get_filename();
    int index = newname.find_last_of(".",newname.size());

    if (arch->type_is_cal())
      newname.replace(index, newname.size(), ".cf");
    else
      newname.replace(index, newname.size(), ".rf");
    
    cerr << "Unloading " << newname << endl;
    fitsarch ->Archive::unload(newname.c_str());
  }
  catch (Error& error) {
    cerr << error << endl;
  }
}
