//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fitsutil/fitsutil.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __FITS_UTIL_H
#define __FITS_UTIL_H

//! Infinity
extern float fits_nullfloat;

//! Convert FITS datatype to string
const char* fits_datatype_str (int datatype);


#endif

