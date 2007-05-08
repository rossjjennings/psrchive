/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "T2Predictor.h"

#include "FITSError.h"

using namespace std;

void unload_T2Predictor (fitsfile* fptr,
			 const Tempo2::Predictor* model, bool verbose)
{
  if (verbose == 3)
    cerr << "unload_T2Predictor entered" << endl;

  // Move to the T2PREDICT HDU
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "T2PREDICT", 0, &status);
  
  if (status)
    throw FITSError (status, "unload_T2Predictor", 
                     "fits_movnam_hdu T2PREDICT");

#if 0  
  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "unload_T2Predictor", 
                     "fits_get_num_rows T2PREDICT");
#endif

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PREDICT", &colnum, &status);

  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (typecode != TSTRING)
    throw Error (InvalidState, "unload_T2Predictor",
		 "PREDICT typecode != TSTRING");

  // a vector cleans itself up (could also use auto_ptr)
  vector<char> text (repeat);
  char* temp = &(text[0]);

  FILE* stream = tmpfile();
  if (!stream)
    throw Error (FailedSys, "unload_T2Predictor", "tmpfile");

  try {
    model->unload(stream);
  }
  catch (Error& error) {
    fclose (stream);
    throw error += "unload_T2Predictor";
  }

  // seek back to the start of the file
  fseek (stream, 0, SEEK_SET);

  int row = 0;
  while (fgets (temp, repeat, stream) == temp)  {
    
    row ++;

    fits_write_col (fptr, TSTRING, colnum, row, 1, 1, &temp, &status);

    if (status) {
      fclose (stream);
      throw FITSError (status, "polynomial::unload",
		       "fits_write_col row=%d", row);
    }

  }

  fclose (stream);

  if (verbose == 3)
    cerr << "unload T2Predictor wrote " << row << " rows" << endl;

}


