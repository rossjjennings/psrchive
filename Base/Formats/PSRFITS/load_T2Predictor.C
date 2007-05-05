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

typedef struct 
{
  fitsfile* fptr;
  int colnum;
  int nrow;
  int row;

  char* buffer;
  int offset;
} fitstream;

int fitstream_readrow (void *, char *, int);

void Pulsar::FITSArchive::load_T2Predictor (fitsfile* fptr)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_T2Predictor entered" << endl;

  // Move to the T2PREDICT HDU
  int status = 0;
  fits_movnam_hdu (fptr, BINARY_TBL, "T2PREDICT", 0, &status);
  
  if (status != 0)
    return;

  long numrows = 0;
  fits_get_num_rows (fptr, &numrows, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_T2Predictor", 
                     "fits_get_num_rows T2PREDICT");

  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PREDICT", &colnum, &status);

  int  typecode = 0;
  long repeat = 0;
  long width = 0;

  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (typecode != TSTRING)
    throw Error (InvalidState, "FITSArchive::load T2Predictor",
		 "PREDICT typecode != TSTRING");

  vector<char> text (repeat);
  char* nul = " ";
  char* temp = &(text[0]);

  fitstream input;
  input.fptr = fptr;
  input.colnum = colnum;
  input.nrow = numrows;
  input.row = 1;
  input.buffer = &(text[0]);
  input.offset = 0;

  FILE* stream = fropen (&input, fitstream_readrow);
  if (!stream)
    throw Error (SysCall, "FITSArchive::load T2Predictor", "fropen");

  Reference::To<Tempo2::Predictor> predictor = new Tempo2::Predictor;

  predictor->load (stream);

  if (verbose == 3)
    cerr << "FITSArchive::load_T2Predictor exiting" << endl;
}

int fitstream_readrow (void* cookie, char* text, int toread)
{
  cerr << "toread=" << toread << endl;

  fitstream* c = (fitstream*) cookie;

  int haveread = 0;

  while (haveread < toread) {

    int left_to_read = toread - haveread;

    if (c->offset == 0) {

      if (c->row > c->nrow)
	break;

      int anynul = 0;
      int status = 0;
      fits_read_col (c->fptr, TSTRING, c->colnum, c->row, 1, 1, 0, 
		     &(c->buffer), &anynul, &status);
      if (status != 0)
	throw FITSError (status, "fitstream_readrow", 
			 "fits_read_col");

      cerr << "loaded\n" << c->buffer << endl;

      c->row ++;
    }

    int length = strlen (c->buffer);
    int available = length - c->offset;

    if (available > left_to_read)
      available = left_to_read;
      
    memcpy (text + haveread, c->buffer + c->offset, available);

    c->offset += available;
    haveread += available;

    left_to_read -= available;

    if (c->offset == length) {

      if (left_to_read) {
	text[haveread] = '\n';
	haveread ++;
      }

      c->offset = 0;
    }
  }
  return haveread;
}
