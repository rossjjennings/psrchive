//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/fitsutil/psrfitsio.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __psrfitsio_h
#define __psrfitsio_h

#include "FITSError.h"
#include <fitsio.h>
#include <string>

//! Empty template class requires specialization 
template<typename T> struct FITS_traits { };
  
//! Template specialization for double
template<> struct FITS_traits<double> {
  static inline int datatype() { return TDOUBLE; }
};

//! Template specialization for float
template<> struct FITS_traits<float> {
  static inline int datatype() { return TFLOAT; }
};

//! Template specialization for int
template<> struct FITS_traits<int> {
  static inline int datatype() { return TINT; }
};

//! Template specialization for long
template<> struct FITS_traits<long> {
  static inline int datatype() { return TLONG; }
};

//! Calls fits_update_key; throws a FITSError exception if status != 0
template<typename T>
void psrfits_update_key (fitsfile* fptr, const char* name, T data)
{
  // no comment
  char* comment = 0;
  // status
  int status = 0;
  
  fits_update_key (fptr, FITS_traits<T>::datatype(),
		   const_cast<char*>(name), &data,
		   comment, &status);
  
  if (status)
    throw FITSError (status, "psrfits_update_key", name);
}

//! Specialization for string
void psrfits_update_key (fitsfile* fptr, const char* name,
			 const std::string& data);

//! Worker function does not handle status
template<typename T>
void psrfits_read_key_work (fitsfile* fptr, const char* name, T* data, 
			    int* status)
{
  // no comment
  char* comment = 0;
  
  fits_read_key (fptr, FITS_traits<T>::datatype(), 
		 const_cast<char*>(name), data,
		 comment, status);
}

//! Specialization for string
void psrfits_read_key_work (fitsfile* fptr, const char* name, std::string*,
			    int* status);

//! Calls fits_read_key; throws a FITSError exception if status != 0
template<typename T>
void psrfits_read_key (fitsfile* fptr, const char* name, T* data)
{
  // status
  int status = 0;
  psrfits_read_key_work (fptr, name, data, &status);
  if (status)
    throw FITSError (status, "psrfits_read_key", name);
}

//! Calls fits_read_key; sets data to default if status != 0
template<typename T>
void psrfits_read_key (fitsfile* fptr, const char* name, T* data,
		       T dfault, bool verbose = false)
{
  // status
  int status = 0;
  psrfits_read_key_work (fptr, name, data, &status);
  if (status) {
    if (verbose) {
      FITSError error (status, "psrfits_read_key", name);
      std::cerr << error.get_message() << std::endl;
      std::cerr << "psrfits_read_key: using default="<< dfault <<std::endl;
    }
    *data = dfault;
  }
}

#endif
