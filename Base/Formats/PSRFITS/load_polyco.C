/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "polyco.h"

using namespace std;

Pulsar::Predictor*
load_polyco (fitsfile* fptr, FITSPolyco* extra, bool verbose)
{
  if (verbose)
    cerr << "load_polyco entered" << endl;

  // Load the polyco from the FITS file

  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "POLYCO", 0, &status);

  if (status) {
    if (verbose)
      cerr << "load_polyco no polyco" << endl;
    return 0;
  }

  Reference::To<polyco> model = new polyco;
  model->load (fptr, extra);

  if (verbose)
    cerr << "load_polyco loaded\n" << *model << endl;

  return model.release();
}
