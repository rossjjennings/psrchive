#include <iostream>
#include <string>

#include <fitsio.h>
#include "psrephem.h"
#include "ephio.h"
#include "Error.h"

int main (int argc, char** argv)
{
  bool verbose = false;
  int arg=1;
  string first = argv[arg];

  if ( argc < 2 || first == "-h" ) {
    cerr << "USAGE: test_fitsio inputfilename" << endl;
    return 0;
  }
  if ( first == "-v" ) {
    psrephem::verbose = true;
    Error::verbose = true;
    verbose = true;
    arg++;
  }

  fitsfile* fptr = 0;
  int status = 0;
  char err[FLEN_STATUS];   // error message if status != 0

  //
  // open PSRFITS file and read ephemeris
  //

  cerr << "Loading ephemeris from PSRFITS file: " << argv[arg] << endl;

  fits_open_file (&fptr, argv[arg], READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  psrephem eph;
  eph.load (fptr);

  cerr << "Ephemeris loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed ephemeris\n" << eph;

  //
  // get PSRFITS file template
  //

  char* psrfits = getenv ("PSRFITS");
  if (!psrfits) {
    cerr << "PSRFITS environment variable not defined.  Cannot continue test."
	 << endl;
    return -1;
  }

  string temp = ".test_psrephem_fitsio.dat";
  string templated = temp + "(" + psrfits + ")";
  
  //
  // create PSRFITS file and write ephemeris
  //

  cerr << "Writing ephemeris to PSRFITS file: " << temp << endl;

  fits_create_file (&fptr, templated.c_str(), &status);
    if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_create_file " << err << endl;
    return -1;
  }

  eph.unload (fptr);

  cerr << "Ephemeris written." << endl;
  fits_close_file (fptr, &status);

  //
  // open newly created PSRFITS file and read ephemeris
  //

  cerr << "Re-loading ephemeris from PSRFITS file: " << temp << endl;

  fits_open_file (&fptr, temp.c_str(), READONLY, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    cerr << "test_fitsio: error fits_open_file " << err << endl;
    return -1;
  }

  psrephem eph2;
  eph2.load (fptr);

  cerr << "Ephemeris re-loaded." << endl;
  fits_close_file (fptr, &status);

  if (verbose)
    cout << "Parsed ephemeris\n" << eph;

  string first, second;

  eph.unload (&first);
  eph2.unload (&second);

  if (second != first)
    cerr << "test_fitsio: FAIL written != read" << endl;

  cerr << "Original\n" << first << "\nWritten\n" << second << endl;

  return 0;
}
