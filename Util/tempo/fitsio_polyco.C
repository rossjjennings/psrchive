#include <iostream>
#include <assert.h>

#include <fitsio.h>

#define PSRFITS 1
#include "poly.h"
#include "genutil.h"
#include "fitsutil.h"
#include "FITSError.h"

// #define DEBUG 1

// ///////////////////////////////////////////////////////////////////////
//
// load a polyco from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// back     - if specified, go back in history  NOT IMPLEMENTED
//

void polyco::load (fitsfile* fptr, int back)
{
#ifdef DEBUG
  cerr << "polyco::load PSRFITS 1" << endl;
#endif

  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0) 
    throw FITSError (status, "polyco::load", "fits_movnam_hdu");

  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "polyco::load", "fits_get_num_rows");

  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;

  int npoly = 0;
  fits_get_colnum (fptr, CASEINSEN, "NPBLK", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, nrows, firstelem, onelement,
		 NULL, &npoly, &anynul, &status);
  if (status != 0)
    throw FITSError (status, "polyco::load", "fits_read_col NPBLK");

  pollys.resize(npoly);

  long row = nrows - npoly + 1;

  for (int ipol=0; ipol<npoly; ipol++)
    pollys[ipol].load (fptr, row+ipol);
}


void polynomial::load (fitsfile* fptr, long row)
{
  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;
  int anynul = 0;
  char* nul = "-";

  int status = 0;          // status returned by FITSIO routines

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 2" << endl;
#endif

  int nspan=0;
  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &nspan, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse NSPAN");

  // set the attribute
  nspan_mins = nspan;

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 2.3" << endl;
#endif

  int ncoef=0;
  fits_get_colnum (fptr, CASEINSEN, "NCOEF", &colnum, &status);
  fits_read_col (fptr, TINT, colnum, row, firstelem, onelement,
		 NULL, &ncoef, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse NCOEF");

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 2.4" << endl;
#endif


#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 2.5" << endl;
#endif

  static char* site = new char[2];
  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  fits_read_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		 nul, &site, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse NSITE");

  // set the attribute
  telescope = site[0];

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 2.6" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "REF_FREQ", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &freq, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse REF_FREQ");

  // freq is the attribute

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 3" << endl;
#endif

  double ref_mjd=0;
  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   NULL, &ref_mjd, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse REF_MJD");

  // set the attribute
  reftime = MJD(ref_mjd);

#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 4" << endl;
#endif

  double ref_phs = 0;
  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &ref_phs, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse REF_PHS");

  // set the attribute
  ref_phase = Phase(ref_phs);
  
  fits_get_colnum (fptr, CASEINSEN, "REF_F0", &colnum, &status);
  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 NULL, &f0, &anynul, &status);
  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse REF_F0");

  // f0 is the attribute

  //! dispersion measure is not included in POLYCO binary table

  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  
#ifdef DEBUG
  cerr << "polynomial::load PSRFITS 5" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat < ncoef)
    throw_str ("polynomial::load COEFF"
	       " vector repeat count=%ld < NCOEF=%d", repeat, ncoef);

  coefs.resize (ncoef);

  fits_read_col (fptr, TDOUBLE, colnum, row, firstelem, ncoef,
		 NULL, coefs.begin(), &anynul, &status);

  if (anynul || status)
    throw FITSError (status, "polynomial::load failed to parse COEFF");

  tempov11 = false;
}


// ///////////////////////////////////////////////////////////////////////
//
// unload a polyco to a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// back     - if specified, go back in history  NOT IMPLEMENTED
//
void polyco::unload (fitsfile* fptr, int back)
{
#ifdef DEBUG
  cerr << "polyco::unload PSRFITS 1" << endl;
#endif

  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);
  if (status != 0) 
    throw FITSError (status, "polyco::unload", "fits_movnam_hdu");

  // ask for the number of rows in the binary table
  long rows = 0;
  fits_get_num_rows (fptr, &rows, &status);
  if (status != 0)
    throw FITSError (status, "polyco::unload", "fits_get_num_rows");

  if (back == 0)
    rows ++;
  else {
    cerr << "polyco::unload WARNING back not implemented. deleting old rows" << endl;
    fits_delete_rows (fptr, 1, rows, &status);
    if (status != 0)
      throw FITSError (status, "polyco::unload", "fits_delete_rows");
    rows = 0;
  }

  int colnum = 0;
  int npoly = pollys.size();
  fits_get_colnum (fptr, CASEINSEN, "NPBLK", &colnum, &status);
  fits_write_col (fptr, TINT, colnum, rows, 1, npoly, &npoly, &status);
  if (status != 0)
    throw FITSError (status, "polyco::unload", "fits_write_col NPBLK");

  for (int ipol=0; ipol<npoly; ipol++)
    pollys[ipol].unload (fptr, rows+ipol);
}





void polynomial::unload (fitsfile* fptr, long row)
{
  // these are used to pull out the data from a cell
  long firstelem = 1;
  long onelement = 1;
  int colnum = 0;

  int status = 0;          // status returned by FITSIO routines

  int nspan = (int) nspan_mins;
  fits_get_colnum (fptr, CASEINSEN, "NSPAN", &colnum, &status);
  fits_write_col (fptr, TINT, colnum, row, firstelem, onelement,
		 &nspan, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col NSPAN");

  int ncoef = coefs.size();
  fits_get_colnum (fptr, CASEINSEN, "NCOEF", &colnum, &status);
  fits_write_col (fptr, TINT, colnum, row, firstelem, onelement,
		 &ncoef, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col NCOEF");

  static char* site = new char[2];
  site[0] = telescope;
  site[1] = '\0';

  fits_get_colnum (fptr, CASEINSEN, "NSITE", &colnum, &status);
  fits_write_col (fptr, TSTRING, colnum, row, firstelem, onelement,
		  &site, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col NSITE");

  fits_get_colnum (fptr, CASEINSEN, "REF_FREQ", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &freq, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col REF_FREQ");

  double ref_mjd=reftime.in_days();
  fits_get_colnum (fptr, CASEINSEN, "REF_MJD", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		   &ref_mjd, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col REF_MJD");

  double ref_phs = ref_phase.in_turns();
  fits_get_colnum (fptr, CASEINSEN, "REF_PHS", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &ref_phs, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col REF_PHS");

  
  fits_get_colnum (fptr, CASEINSEN, "REF_F0", &colnum, &status);
  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, onelement,
		 &f0, &status);
  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col REF_F0");

  //! dispersion measure is not included in POLYCO binary table

  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  
#ifdef DEBUG
  cerr << "polynomial::unload PSRFITS 5" << endl;
#endif

  fits_get_colnum (fptr, CASEINSEN, "COEFF", &colnum, &status);
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  
  if (repeat < ncoef)
    throw Error (InvalidState, "polynomial::unload",
	       "COEFF vector repeat count=%ld < NCOEF=%d", repeat, ncoef);

  fits_write_col (fptr, TDOUBLE, colnum, row, firstelem, ncoef,
		 coefs.begin(), &status);

  if (status)
    throw FITSError (status, "polynomial::unload", "fits_write_col COEFF");
}

