
#include <iostream>
#include <vector>
#include <assert.h>

#include <fitsio.h>

#include "psrephem.h"
#include "ephio.h"
#include "genutil.h"
#include "coord.h"

// ///////////////////////////////////////////////////////////////////////
//
// load an ephemeris from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - if specified, parse the row'th record in the binary table
//            otherwise, parse the last (most recent) row
//

const char* fits_datatype_str (int datatype)
{
  switch (datatype) {

  case TBIT:
    return "bit 'X'";
  case TBYTE:
    return "byte 'B'";
  case TLOGICAL:
    return "logical 'L'";
  case TSTRING:
    return "string 'A'";
  case TSHORT:
    return "short 'I'";
  case TINT32BIT:
    return "32-bit int 'J'";
    //  case TLONG:
    //  return "long";
  case TFLOAT:
    return "float 'E'";
  case TDOUBLE:
    return "double 'D'";
  case TCOMPLEX:
    return "complex 'C'";
  case TDBLCOMPLEX:
    return "double complex 'M'";
  case TINT:
    return "int";
  case TUINT:
    return "unsigned int";
  case TUSHORT:
    return "unsigned short";
  case TULONG:
    return "unsigned long";
  default:
    return "unknown";
  }

  return "unknown";
}

void datatype_match (int typecode, int ephind)
{
  int parmtype = parmTypes[ephind];
  assert (parmtype>=0 && parmtype<6);

  // the CFITSIO datatype corresponding to each parmType
  static int datatype[6] = { TSTRING,  // string
			     TDOUBLE,  // double
			     TSTRING,  // h:m:s
			     TSTRING,  // d:m:s
			     TDOUBLE,  // MJD
			     TSHORT }; // integer
  
  cerr << "typecode=" << typecode << " datatype[" << parmtype << "]="
       << datatype[parmtype] << endl;
  
  if (typecode != datatype[parmtype])
    fprintf (stderr, 
	     "psrephem::load PSRFITS binary table element datatype:%s\n" 
	     "doesn't match parmType:%d for %s\n", 
	     fits_datatype_str(typecode), parmtype,
	     parmNames[ephind]);
}

void psrephem::load (fitsfile* fptr, long row)
{
  int status = 0;          // status returned by FITSIO routines
  char err[FLEN_STATUS];   // error message if status != 0

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    throw_str ("psrephem::load error fits_movnam_hdu %s", err);
  }

  // ask for the number of rows in the binary table
  long nrows = 0;
  int  ncols = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  fits_get_num_cols (fptr, &ncols, &status);
  if (status != 0) {
    fits_get_errstatus (status, err);
    throw_str ("psrephem::load error fits_get_num_[row|col]s %s", err);
  }

  if (row > nrows || row < 1)
    row = nrows;

  if (verbose)
    cerr << "psrephem::load PSRFITS nrows=" << nrows << " ncols=" << ncols 
	 << endl;

  //
  // construct a mapping between the column number and the ephio index
  //

  // get ready to parse the values
  tempo11 = 1;
  size_dataspace();

  vector<int> ephind (ncols);   // ephio index for column number

  string parstr;

  char keyword [FLEN_KEYWORD+1];
  char value   [FLEN_VALUE+1];
  char comment [FLEN_COMMENT+1];
  int  typecode = 0;
  long repeat = 0;
  long width = 0;
  int  maxstrlen = 0;
  int  icol = 0;

  for (icol=0; icol<ncols && status==0; icol++) {

    fits_get_coltype (fptr, icol+1, &typecode, &repeat, &width, &status);

    sprintf(keyword, "TTYPE%d", icol+1);
    fits_read_key (fptr, TSTRING, keyword, value, comment, &status);

    if (verbose)
      cerr << icol+1 << " '" << value << "' repeat=" << repeat 
	   << " width=" << width << " typecode=" << typecode 
	   << " comment=" << comment << endl;
    
    parstr = value;

    if ( parstr == "PSR-NAME" )
      parstr = "PSRJ";
    
    ephind[icol] = -1;

    for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++) {
      
      if (parstr == parmNames[ieph]) {

	// match found ... 
	// test the validity of operating assumption.  datatype match
	// will display an error if there is a mismatch in assumption
	// datatype_match (typecode, ieph);

	ephind[icol] = ieph;

	// check the amount of space required to read
	if (typecode == TSTRING && repeat > maxstrlen)
	  maxstrlen = repeat;
	
	break;
      }
    }
  }

  char* strval = new char [maxstrlen + 1];

  for (icol=0; icol<ncols && status==0; icol++) {

    int ieph = ephind[icol];
    if (ieph < 0)
      continue;

    long firstelem = 1;
    long nelements = 1;
    int anynul = 0;

    switch (parmTypes[ieph]) {

    case 0:  // string
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, nelements,
		       nul, strval, &anynul, &status);
	cerr << "READ STR=" << strval << endl;
	value_str[ieph] = strval;
	break;
      }

    case 1:  // double
      {
	double nul = 0.0;
	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, nelements,
		       &nul, value_double + ieph, &anynul, &status);
	break;
      }
    case 2:  // h:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, nelements,
		       nul, strval, &anynul, &status);
	cerr << "READ RA=" << strval << endl;

	if (str2ra (value_double + ieph, strval) != 0)
	  cerr << "psrephem::load PSRFITS could not parse h:m:s from '" 
	       << strval << "'" << endl;
	break;
      }
    case 3:  // d:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, nelements,
		       nul, strval, &anynul, &status);
	cerr << "READ DEC=" << strval << endl;

	if (str2dec (value_double + ieph, strval) != 0)
	  cerr << "psrephem::load PSRFITS could not parse d:m:s from '" 
	       << strval << "'" << endl;
	break;
      }
    case 4:  // MJD
      {
	double nul = 0.0;
	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, nelements,
		       &nul, value_double + ieph, &anynul, &status);

	value_integer[ieph] = value_double[ieph];
	value_double[ieph] -= value_integer[ieph];
	break;
      }
    case 5:  // integer
      {
	int nul = 0;
	fits_read_col (fptr, TINT, icol+1, row, firstelem, nelements,
		       &nul, value_integer + ieph, &anynul, &status);
	break;
      }
    default:
      cerr << "psrephem::load PSRFITS invalid parmType" << endl;
	break;
    }

    if (status == 0)
      parmStatus[ieph] = 1;
    else {
      fits_get_errstatus (status, err);
      cerr << "psrephem::load PSRFITS error parsing " << parmNames[ieph]
	   << " - " << err << endl;
      status = 0;
    }

    if (anynul)
      parmStatus[ieph] = 0;

  }

  if (icol < ncols)
    cerr << "psrephem::load PSRFITS read " << icol <<"/" << ncols 
	 << " elements" << endl;

}

