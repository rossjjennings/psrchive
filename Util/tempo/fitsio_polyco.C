#include <iostream>
#include <assert.h>

#include <fitsio.h>

#define PSRFITS 1
#include "poly.h"
#include "genutil.h"
#include "fitsutil.h"

// #define DEBUG 1

// ///////////////////////////////////////////////////////////////////////
//
// load a polyco from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - if specified, parse the row'th record in the binary table
//            otherwise, parse the last (most recent) row
//

void polyco::load (fitsfile* fptr, long row)
{
#ifdef DEBUG
  cerr << "polyco::load PSRFITS 1" << endl;
#endif

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
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    throw_str ("polyco::load error fits_get_num_[row|col]s %s", err);
  }

  if (row > nrows || row < 1)
    row = nrows;

  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;
  char* nul = "-";

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 2" << endl;
#endif

  int nspan=0;
  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &nspan, &anynul, &status);
  if (anynul || status)
    throw string ("polyco::load failed to parse NSPAN");

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 2.3" << endl;
#endif

  int ncoef=0;
  fits_get_colnum (fptr, CASEINSEN, "NCOEF", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &ncoef, &anynul, &status);
  if (anynul || status)
    throw string ("polyco::load failed to parse NCOEF");

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 2.4" << endl;
#endif

  int npoly=0;
  fits_get_colnum (fptr, CASEINSEN, "NPBLK", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &npoly, &anynul, &status);
  if (anynul || status)
    throw string ("polyco::load failed to parse NPBLK");

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 2.5" << endl;
#endif

  char* site = new char[2];
  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  fits_read_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		 nul, &site, &anynul, &status);
  if (anynul || status)
    throw string ("polyco::load failed to parse NSITE");

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 2.6" << endl;
#endif
 
  double ref_freq=0;
  fits_get_colnum (fptr, CASEINSEN, "REF_FREQ", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &ref_freq, &anynul, &status);
  if (anynul || status)
    throw string ("polyco::load failed to parse NSPAN");

  if (verbose)
    cerr << "polcy::load PSRFITS " << npoly << " polynomial blocks" << endl;

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 3" << endl;
#endif

  // these are used to query the nature of the COEFF column
  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  pollys.resize(npoly);

  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat != npoly)
    throw_str ("polynomial::load REF_MJD vector repeat count=%ld != NPBLK=%d",
	       repeat, npoly);

  int ipoly = 0;

  for (ipoly=0; ipoly<npoly; ipoly++) {
    double temp=0;
    firstelem = ipoly + 1;    
    fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   NULL, &temp, &anynul, &status);
    pollys[ipoly].reftime = MJD(temp);
  }

  if (anynul || status)
    throw string ("polynomial::load failed to parse REF_MJD");

#ifdef DEBUG
  cerr << "polyco::load PSRFITS 4" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat != npoly)
    throw_str ("polynomial::load REF_PHS vector repeat count=%ld != NPBLK=%d",
	       repeat, npoly);
  
  for (ipoly=0; ipoly<npoly; ipoly++) {
    double temp=0;
    firstelem = ipoly + 1;    
    fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   NULL, &temp, &anynul, &status);
    pollys[ipoly].ref_phase = Phase(temp);
  }

  if (anynul || status)
    throw string ("polynomial::load failed to parse REF_PHS");

  
#ifdef DEBUG
  cerr << "polyco::load PSRFITS 5" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat != ncoef * npoly)
    throw_str ("polynomial::load COEFF"
	       " vector repeat count=%ld != NCOEF*NPBLK=%d",
	       repeat, ncoef * npoly);

  for (ipoly=0; ipoly<npoly; ipoly++) {

    pollys[ipoly].coefs.resize(ncoef);

    firstelem = ipoly*ncoef + 1;

    fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, ncoef,
		   NULL, pollys[ipoly].coefs.begin(), &anynul, &status);

  }

  for (ipoly=0; ipoly<npoly; ipoly++) {
    pollys[ipoly].telescope = site[0];
    pollys[ipoly].freq = ref_freq;
    pollys[ipoly].nspan_mins = (double) nspan;
  }

  delete [] site;
}
