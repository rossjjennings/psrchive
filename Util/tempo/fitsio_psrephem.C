#include <iostream>
#include <vector>
#include <algorithm>

#include <assert.h>

#include <fitsio.h>

#define PSRFITS 1
#include "psrephem.h"
#include "ephio.h"

#include "FITSError.h"
#include "fitsutil.h"
#include "coord.h"
#include "genutil.h"

#include "f772c.h"

extern "C" {
  // these are defined in ephio.f
  int F772C(turnstodms) (double* turns, char* dms);
  int F772C(turnstohms) (double* turns, char* hms);
}

// utility function defined at the end of the file
void datatype_match (int typecode, int ephind);


/*! This method prepares a mapping between the columns of the PSREPHEM hdu
  and the indeces used internally.

  \pre fptr must refer to a PSRFITS file with a PSREPHEM hdu 
  \post fptr will be left set to the PSREPHEM hdu
*/

void psrephem::fits_map (fitsfile* fptr, vector<int>& ephind, int& maxstrlen)
  const
{
  int status = 0;          // status returned by FITSIO routines

  // move to the appropriate header+data unit
  fits_movnam_hdu (fptr, BINARY_TBL, "PSREPHEM", 0, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::fits_map", "fits_movnam_hdu");

  // ask for the number of columns in the binary table
  int ncols = 0;
  fits_get_num_cols (fptr, &ncols, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::fits_map", "fits_get_num_cols");

  if (verbose)
    cerr << "psrephem::fits_map ncols=" << ncols << endl;

  //
  // construct a mapping between the column number and the ephio index
  //
  ephind.resize (ncols);

  string parstr;

  char keyword [FLEN_KEYWORD+1];
  char value   [FLEN_VALUE+1];
  char comment [FLEN_COMMENT+1];

  maxstrlen = 0;

  string previous;

  for (int icol=0; icol<ncols && status==0; icol++) {

    int  typecode = 0;
    long repeat = 0;
    long width = 0;
    fits_get_coltype (fptr, icol+1, &typecode, &repeat, &width, &status);

    fits_make_keyn ("TTYPE", icol+1, keyword, &status);
    fits_read_key (fptr, TSTRING, keyword, value, comment, &status);

    if (verbose)
      cerr << icol+1 << " '" << value << "' repeat=" << repeat 
	   << " width=" << width << " typecode=" << typecode
	   << " comment=" << comment << endl;
    
    parstr = value;

    // standardize the various strings encountered to date
    if ( parstr == "PSR-NAME" || parstr == "PSR_NAME" )
      parstr = "PSRJ";

    //
    // columns containing the fractional component must be preceded by integer
    //
    if ( parstr == "TZRFMJD" ) {
      if (previous != "TZRIMJD")
	throw Error (InvalidState, "psrephem::fits_map",
		     "TZRFMJD does not follow TZRIMJD");
      parstr = "TZRMJD";
    }

    if ( parstr == "FF0" ) {
      if (previous != "IF0")
	throw Error (InvalidState, "psrephem::fits_map",
		     "FF0 does not follow IF0");
      parstr = "F";
    }

    previous = parstr;

    //
    // search for a match
    //
    ephind[icol] = -1;

    for (int ieph=0; ieph<EPH_NUM_KEYS; ieph++) {
      
      if (parstr == parmNames[ieph]) {

	// match found ... 
	// test the validity of operating assumption.  datatype match
	// will display an error if there is a mismatch in assumption
	if (verbose)
	  cerr << "psrephem::fit_map column " << icol
	       << " matches ephind " << ieph << " ";

	datatype_match (typecode, ieph);

	ephind[icol] = ieph;

	// check the amount of space required to read
	if (typecode == TSTRING && repeat > maxstrlen)
	  maxstrlen = repeat;
	
	break;
      }
    }

  }

}






// ///////////////////////////////////////////////////////////////////////
//
// load an ephemeris from a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
// row      - if specified, parse the row'th record in the binary table
//            otherwise, parse the last (most recent) row
//

void psrephem::load (fitsfile* fptr, long row)
{
  vector<int> ephind;   // ephio index for column number
  int maxstrlen = 0;

  // when fits_map returns, fptr will be set to the PSREPHEM hdu
  fits_map (fptr, ephind, maxstrlen);

  int status = 0;          // status returned by FITSIO routines

  // ask for the number of rows in the binary table
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::load", "fits_get_num_rows");

  if (row > nrows || row < 1)
    row = nrows;

  if (verbose)
    cerr << "psrephem::load PSRFITS nrows=" << nrows << " ncols=" 
	 << ephind.size() << endl;

  // get ready to parse the values
  tempo11 = 1;
  size_dataspace();


  char* strval = new char [maxstrlen+1];
  unsigned icol=0;

  for (icol=0; icol<ephind.size() && status==0; icol++) {

    int ieph = ephind[icol];
    if (ieph < 0)
      continue;

    long firstelem = 1;
    long onelement = 1;
    int anynul = 0;

    switch (parmTypes[ieph]) {

    case 0:  // string
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       nul, &strval, &anynul, &status);
	value_str[ieph] = strval;
	break;
      }

    case 1:  // double
      {
	double nul = 0.0;
	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
		       &nul, value_double + ieph, &anynul, &status);
	break;


	if (ieph == EPH_F) {
	  int inul = 0;
	  // special case for FF0:
	  // Assumes that IF0 is in the column preceding FF0
	  fits_read_col (fptr, TINT, icol, row, firstelem, onelement,
			 &inul, value_integer + ieph, &anynul, &status);

	  // for now, this class represents F as a double
	  value_double[ieph] += value_integer[ieph];
	  value_integer[ieph] = 0;
	}

      }
    case 2:  // h:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       nul, &strval, &anynul, &status);

	if (verbose)
	  cerr << "psrephem::load PSRFITS h:m:s=" << strval << endl;

	if (str2ra (value_double + ieph, strval) != 0) {
	  if (verbose)
	    cerr << "psrephem::load PSRFITS could not parse h:m:s from '" 
		 << strval << "'" << endl;
	  anynul = 1;
	}

	// convert from radians to turns
	value_double[ieph] /= 2.0 * M_PI;

	break;
      }
    case 3:  // d:m:s
      {
	char* nul = " ";
	fits_read_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
		       nul, &strval, &anynul, &status);

	if (verbose)
	  cerr << "psrephem::load PSRFITS d:m:s=" << strval << endl;

	if (str2dec (value_double + ieph, strval) != 0) {
	  if (verbose)
	    cerr << "psrephem::load PSRFITS could not parse d:m:s from '" 
		 << strval << "'" << endl;
	  anynul = 1;
	}

	// convert from radians to turns
	value_double[ieph] /= 2.0 * M_PI;

	break;
      }
    case 4:  // MJD
      {
	double nul = 0.0;
	fits_read_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
		       &nul, value_double + ieph, &anynul, &status);

	if (ieph == EPH_TZRMJD) {
	  int inul = 0;
	  // special case for TZRMJD:
	  // Assumes that TZRIMJD is in the column preceding TZRFMJD
	  fits_read_col (fptr, TINT, icol, row, firstelem, onelement,
			 &inul, value_integer + ieph, &anynul, &status);
	}
	else {
	  // separate the integer
	  value_integer[ieph] = int (value_double[ieph]);  // truncate
	  value_double[ieph] -= value_integer[ieph];
	}
	break;
      }
    case 5:  // integer
      {
	int nul = 0;
	fits_read_col (fptr, TINT, icol+1, row, firstelem, onelement,
		       &nul, value_integer + ieph, &anynul, &status);
	break;
      }
    default:
      cerr << "psrephem::load PSRFITS invalid parmType" << endl;
	break;
    }

    if (status != 0)
      throw FITSError (status, "psrephem::load", 
		       "error parsing %s", parmNames[ieph]);

    if (anynul != 0)
      parmStatus[ieph] = 0;
    else
      parmStatus[ieph] = 1;

  }

  delete strval;

  if (icol < ephind.size())
    cerr << "psrephem::load PSRFITS read " << icol <<"/" << ephind.size()
	 << " elements" << endl;

}






// ///////////////////////////////////////////////////////////////////////
//
// unload an ephemeris to the PSREPHEM hdu of a PSRFITS file
//
// fitsfile - points to an open PSRFITS archive
//
// row      - if specified, unload to the row'th record in the binary table
//            otherwise, create a new row and unload to this one
//
void psrephem::unload (fitsfile* fptr, long row) const
{
  vector<int> ephind;   // ephio index for column number
  int maxstrlen;

  // when fits_map returns, fptr will be set to the PSREPHEM hdu
  fits_map (fptr, ephind, maxstrlen);

  // check that all of the required columns exist in the table
  for (int ieph=0; ieph < EPH_NUM_KEYS; ieph++)
    if (parmStatus[ieph] &&
	find (ephind.begin(), ephind.end(), ieph) == ephind.end())
      throw Error (InvalidState, "psrephem::unload",
		   "TO-DO: create %s column in PSREPHEM hdu", parmNames[ieph]);
  
  // ask for the number of rows in the binary table
  int status = 0;          // status returned by FITSIO routines
  long nrows = 0;
  fits_get_num_rows (fptr, &nrows, &status);
  if (status != 0)
    throw FITSError (status, "psrephem::unload", "fits_get_num_rows");

  if (row > nrows)
    throw Error (InvalidRange, "psrephem::unload",
		 "row=%d > nrows=%d", row, nrows);

  if (row < 1) {
    // row unspecified
    if (verbose)
      cerr << "psrephem::unload adding new row to PSREPHEM hdu" << endl;
    nrows ++;
    row = nrows;
  }

  if (verbose)
    cerr << "psrephem::unload PSRFITS nrows=" << nrows << " ncols=" 
	 << ephind.size() << endl;

  for (unsigned icol=0; icol<ephind.size() && status==0; icol++) {

    int ieph = ephind[icol];
    if (ieph < 0)
      continue;

    long firstelem = 1;
    long onelement = 1;
 
    if (parmStatus[ieph] == 0) {
      // NULLIFY entry, if any
      fits_write_col_null (fptr, icol+1, row, firstelem, onelement, &status);
      if (status)
	throw FITSError (status, "psrephem::unload", "fits_write_col_null");
      continue;
    }

    switch (parmTypes[ieph]) {

    case 0:  // string
      {
	char* value = const_cast<char*>( value_str[ieph].c_str() );
	fits_write_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }

    case 1:  // double
      {
	double value = value_double[ieph];

	if (ieph == EPH_F) {
	  // special case for FF0:
	  // Assumes that IF0 is in the column preceding FF0
	  int intval = int (value);

	  fits_write_col (fptr, TINT, icol, row, firstelem, onelement,
			  &intval, &status);

	  value -= intval;
	}

	fits_write_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }
    case 2:  // h:m:s
      {
	char value [64];
	F772C(turnstohms) (value_double + ieph, value);
	fits_write_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }
    case 3:  // d:m:s
      {
	char value [64];
	F772C(turnstodms) (value_double + ieph, value);
	fits_write_col (fptr, TSTRING, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }
    case 4:  // MJD
      {
	double value = value_double[ieph];

	if (ieph == EPH_TZRMJD) {
	  // special case for TZRMJD:
	  // Assumes that TZRIMJD is in the column preceding TZRFMJD
	  fits_write_col (fptr, TINT, icol, row, firstelem, onelement,
			  value_integer + ieph, &status);
	}
	else
	  // add the integer
	  value += value_integer[ieph];

	fits_write_col (fptr, TDOUBLE, icol+1, row, firstelem, onelement,
			&value, &status);
	break;
      }
    case 5:  // integer
      {
	fits_write_col (fptr, TINT, icol+1, row, firstelem, onelement,
			value_integer + ieph, &status);
	break;
      }

    default:
      throw Error (InvalidState, "psrephem::unload",
		   "invalid parmType[%d]", ieph);

    }
    
    if (status == 0)
      throw FITSError (status, "psrephem::unload", "fits_write_col");
  }
}




void datatype_match (int typecode, int ephind)
{
  int parmtype = parmTypes[ephind];
  assert (parmtype>=0 && parmtype<6);

  // the CFITSIO datatype corresponding to each parmType
  int datatype[7] = { TSTRING,  // string
		      TDOUBLE,  // double
		      TSTRING,  // h:m:s
		      TSTRING,  // d:m:s
		      TDOUBLE,  // MJD
		      TSHORT }; // integer
  
  if (psrephem::verbose)
    cerr << "typecode=" << typecode << " datatype[" << parmtype << "]="
	 << datatype[parmtype] << endl;
  
  if (typecode != datatype[parmtype])
    throw Error (InvalidState, "psrephem::datatype_match",
		 "PSRFITS binary table column datatype:%s\n" 
		 "doesn't match parmType:%d for %s\n", 
		 fits_datatype_str(typecode), parmtype,
		 parmNames[ephind]);
}
