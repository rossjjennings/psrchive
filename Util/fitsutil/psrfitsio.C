/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "psrfitsio.h"

using namespace std;

void psrfits_clean_rows (fitsfile* ffptr)
{
  long rows = 0;
  int status = 0;

  fits_get_num_rows (ffptr, &rows, &status);

  if (status)
    throw FITSError (status, "psrfits_clean_rows", "fits_get_num_rows");

  if (!rows)
    return;

  fits_delete_rows (ffptr, 1, rows, &status);

  if (status)
    throw FITSError (status, "psrfits_clean_rows", "fits_delete_rows");
}

void psrfits_update_key (fitsfile* fptr, const char* name, const char* data)
{
  psrfits_update_key (fptr, name, string(data));
}

//! Specialization for string
void psrfits_update_key (fitsfile* fptr, const char* name, const string& txt)
{
  // no comment
  char* comment = 0;
  // status
  int status = 0;
  
  fits_update_key (fptr, TSTRING, const_cast<char*>(name), 
		   const_cast<char*>(txt.c_str()),
		   comment, &status);
  
  if (status)
    throw FITSError (status, "psrfits_update_key", name);
}

void psrfits_read_key_work (fitsfile* fptr, const char* name, string* data,
			    int* status)
{
  // no comment
  char* comment = 0;

  char temp [FLEN_VALUE];

  fits_read_key (fptr, TSTRING, const_cast<char*>(name), temp, 
		 comment, status);

  if (*status == 0)
    *data = temp;
}

//! Specialization for string
void psrfits_read_col_work( fitsfile *fptr, const char *name,
			    string *data,
			    int row, string& null, int* status)
{
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, const_cast<char*>(name), &colnum, status);
 
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, status);
  if (*status != 0)
    return; 
 
  char* nullstr = const_cast<char*>( null.c_str() );
  
  auto_ptr<char> temp( new char[repeat + 1] );
  char* temp_ptr = temp.get();

  int anynul = 0;
  fits_read_col( fptr, TSTRING,
		 colnum, row,
		 1, 1, &nullstr, &temp_ptr, 
		 &anynul, status );

  if (*status == 0)
    *data = temp.get();
}

void* FITS_void_ptr (const string& txt)
{
  // not thread-safe
  static char* ptr;
  ptr = const_cast<char*> (txt.c_str());
  return &ptr;
}

