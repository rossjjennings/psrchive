#include <iostream>
#include <string>

#include <fitsio.h>
#include "psrephem.h"
#include "ephio.h"
#include "Error.h"

int main (int argc, char** argv)
{
  int arg=1;
  string first = argv[arg];

  if ( argc < 2 || first == "-h" ) {
    cerr << "USAGE: test_fitsio inputfilename" << endl;
    return 0;
  }
  if ( first == "-v" ) {
    psrephem::verbose = true;
    Error::verbose = true;
    arg++;
  }

  // 
  psrephem eph;
  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0


  //
  // open PSRFITS file and read ephemeris
  //
  fits_open_file (&fptr, argv[arg], READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  eph.load (fptr);
  fits_close_file (fptr, &status);

  cout << "Parsed ephemeris\n" << eph;

  //
  // create PSRFITS file and write ephemeris
  //
  char* psrfits = getenv ("PSRFITS");
  if (!psrfits) {
    cerr << "PSRFITS not defined" << endl;
    return -1;
  }

  string temp = ".test_psrephem_fitsio.dat";
  string templated = temp + "(" + psrfits + ")";
  
  fits_create_file (&fptr, templated.c_str(), &status);
    if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_create_file " << err << endl;
    return -1;
  }

  cerr << "Writing ephemeris to PSRFITS file: " << temp << endl;
  eph.unload (fptr);
  fits_close_file (fptr, &status);

  //
  // open newly created PSRFITS file and read ephemeris
  //
  fits_open_file (&fptr, temp.c_str(), READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  cerr << "Reading ephemeris from PSRFITS file: " << temp << endl;

  psrephem written;
  written.load (fptr);
  fits_close_file (fptr, &status);

  cout << "Parsed ephemeris\n" << eph;

  if (written != eph)
    cerr << "test_fitsio: FAIL written != read" << endl;

  return 0;
}
