#include <iostream>
#include <string>

#include <fitsio.h>
#include "psrephem.h"
#include "ephio.h"

int main (int argc, char** argv)
{
  int arg=1;
  string first = argv[arg];

  if ( argc < 2 || first == "-h" ) {
    cerr << "USAGE: test_fitsio filename [row]" << endl;
    return 0;
  }
  if ( first == "-v" ) {
    psrephem::verbose = true;
    arg++;
  }

  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0

  fits_open_file (&fptr, argv[arg], READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  // psrephem::verbose = true;
  psrephem eph;

  eph.load (fptr);

  cout << "Parsed ephemeris\n" << eph;

  fits_close_file (fptr, &status);

  return 0;
}
