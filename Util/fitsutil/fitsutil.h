//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/fitsutil/fitsutil.h,v $
   $Revision: 1.1 $
   $Date: 2003/01/20 09:11:40 $
   $Author: straten $ */

#ifndef __FITS_UTIL_H
#define __FITS_UTIL_H

#include <fitsio.h>

static const char* fits_datatype_str (int datatype)
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

#ifdef linux
  case TINT32BIT:
    return "32-bit int 'J'";
#endif

#ifdef __alpha
    case TLONG:
    return "long 'J'";
#endif

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

}

#endif

