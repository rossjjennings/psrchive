#include <iostream>
#include <assert.h>

#include <fitsio.h>

#include "poly.h"
#include "genutil.h"
#include "fitsutil.h"

// ///////////////////////////////////////////////////////////////////////
//
// load a polynomial from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - if specified, the row in the binary table
//
void polyco::load (fitsfile* fptr, long row)
{
  int status = 0;          // status returned by FITSIO routines
  char err[FLEN_STATUS];   // error message if status != 0

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    throw_str ("polyco::load error fits_movnam_hdu %s", err);
  }

  // ask for the number of rows in the binary table
  long nrows = 0;
  int  ncols = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  fits_get_num_cols (fptr, &ncols, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    throw_str ("polyco::load error fits_get_num_[row|col]s %s", err);
  }

  if (row > nrows || row < 1)
    row = nrows;

  pollys.resize(1);

  if (verbose)
    cerr << "polyco::load PSRFITS call polynomial load" << endl;
  pollys[0].load (fptr, row);
}

// ///////////////////////////////////////////////////////////////////////
//
// load a polynomial from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - the row in the binary table
//
// assumes that the CHDU is name POLYCO
//
void polynomial::load (fitsfile* fptr, long row)
{
  int status = 0;          // status returned by FITSIO routines
  char err[FLEN_STATUS];   // error message if status != 0

  tempov11 = false;

  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;

  // these are used to query the nature of a column
  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &nspan_mins, &anynul, &status);
  if (anynul || status)
    throw string ("polynomial::load failed to parse NSPAN");

  int ncoefs;
  fits_get_colnum (fptr, CASEINSEN, "NCOEFF", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &ncoefs, &anynul, &status);
  if (anynul || status)
    throw string ("polynomial::load failed to parse NCOEFF");

  coefs.resize(ncoefs);

  char* site = new char[2];
  char* nul = "-";

  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);

  if (polyco::verbose) {
    fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);
    cerr << "polynomial::load PSRFITS NSITE repeat=" << repeat 
	 << " width=" << width << " type=" << fits_datatype_str(typecode)
	 << endl;
  }

  fits_read_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		 nul, &site, &anynul, &status);

  if (anynul || status)
    throw string ("polynomial::load failed to parse NSITE");
 
  telescope = site[0];
  delete [] site;

  double temp;
  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &temp, &anynul, &status);
  if (anynul || status)
    throw string ("polynomial::load failed to parse REF_MJD");

  reftime = MJD(temp);

  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &temp, &anynul, &status);

  if (anynul || status)
    throw string ("polynomial::load failed to parse REF_PHS");

  ref_phase = Phase(temp);

  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);

  if (polyco::verbose)
    cerr << "polynomial::load PSRFITS COEFF repeat=" << repeat 
	 << " width=" << width << " type=" << fits_datatype_str(typecode)
	 << endl;
  
  if (repeat != ncoefs)
    throw_str ("polynomial::load COEFF vector repeat count=%ld != NCOEFF=%d",
	       repeat, ncoefs);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, repeat,
		 NULL, coefs.begin(), &anynul, &status);
  
  if (anynul || status)
    throw_str ("polynomial::load failed to parse COEFF %ld vector", repeat);

}
