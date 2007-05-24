/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "FITSError.h"

#include <fitsio.h>

using namespace std;

Pulsar::Predictor* load_T2Predictor (fitsfile* fptr, bool verbose)
{
  if (verbose)
    cerr << "load_T2Predictor entered" << endl;

  // Move to the T2PREDICT HDU
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "T2PREDICT", 0, &status);
  
  if (status != 0)
    return 0;

  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "load_T2Predictor", 
                     "fits_get_num_rows T2PREDICT");

  if (!numrows)
    return 0;

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PREDICT", &colnum, &status);

  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (typecode != TSTRING)
    throw Error (InvalidState, "load_T2Predictor",
		 "PREDICT typecode != TSTRING");

  vector<char> text (repeat);
  char* temp = &(text[0]);

  FILE* stream = tmpfile();

  for (int row=1; row <= numrows; row++) {

    int anynul = 0;
    fits_read_col (fptr, TSTRING, colnum, row, 1, 1, 0, 
                   &temp, &anynul, &status);
    if (status)
      throw FITSError (status, "load_T2Predictor",
		       "fits_read_col");

    fprintf (stream, "%s\n", temp);
  }

  fseek (stream, 0, SEEK_SET);

  Reference::To<Tempo2::Predictor> predictor = new Tempo2::Predictor;
  predictor->load (stream);

  fclose (stream);

  if (verbose) {
    cerr << "load_T2Predictor loaded" << endl;
    predictor->unload (stderr);
  }

  return predictor.release();
}


