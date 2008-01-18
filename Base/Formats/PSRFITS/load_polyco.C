/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "fitsio_tempo.h"
#include "FITSError.h"

#include <float.h>

Pulsar::Predictor*
load_polyco (fitsfile* fptr, double* pred_phs,
	     bool constant_period, bool verbose)
{
  if (verbose)
    cerr << "load_polyco entered" << endl;

  // Load the polyco from the FITS file

  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);

  if (status) {
    if (verbose)
      cerr << "load_polyco no POLYCO HDU" << endl;
    return 0;
  }

  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "load_polyco", "fits_get_num_rows");

  if (nrows == 0) {
    if (verbose)
      cerr << "load_polyco no rows in POLYCO HDU" << endl;
    return 0;
  }

  Reference::To<polyco> model = new polyco;
  load (fptr, model);

  if (verbose)
    cerr << "load_polyco loaded\n" << *model << endl;

  if (pred_phs) {

    // ask for the number of rows in the binary table
    long nrows = 0;
    fits_get_num_rows (fptr, &nrows, &status);
    if (status != 0)
      throw FITSError (status, "load_polyco", "fits_get_num_rows");
    
    long firstelem = 1;
    long onelement = 1;
    int colnum = 0;
    int anynul = 0;
    fits_get_colnum (fptr, CASEINSEN, "PRED_PHS", &colnum, &status);
    fits_read_col (fptr, TDOUBLE, colnum, nrows, firstelem, onelement,
		   NULL, pred_phs, &anynul, &status);

  }

  if (constant_period)
  {
    /*
      If a polyco is used to represent a CAL observation with constant
      period, and that calibrator is added to another, then it is
      possible for the epoch of the integrated result to fall outside
      of the span of the polyco that it written to file.  The
      following code corrects this condition by setting the span to a
      very large number.
    */
    for (unsigned ipoly=0; ipoly < model->pollys.size(); ipoly++)
    {
      polynomial::Expert expert( &(model->pollys[ipoly]) );

      // multiply by 0.99 in case FLT_MAX means anything special anywhere
      expert.set_nspan( FLT_MAX * 0.99 );
    }
  }
  


  return model.release();
}
