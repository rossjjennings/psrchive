#include <iostream>
#include <string>

#include <fitsio.h>
#include "psrephem.h"
#include "ephio.h"

int main (int argc, char** argv)
{
  if ( argc < 2 || argv[1]==string("-h") ) {
    cerr << "USAGE: test_fitsio filename [row]" << endl;
    return 0;
  }

  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0

  fits_open_file (&fptr, argv[1], READONLY, &status);
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
