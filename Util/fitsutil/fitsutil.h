//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/fitsutil/fitsutil.h

#ifndef __FITS_UTIL_H
#define __FITS_UTIL_H

//! Infinity
extern float fits_nullfloat;

//! Convert FITS datatype to string
const char* fits_datatype_str (int datatype);

//! Checks that library and header version numbers match
void fits_version_check (bool verbose = false);

#endif

