/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/environ.h,v $
   $Revision: 1.17 $
   $Date: 2006/11/17 11:57:12 $
   $Author: straten $ */

/*
 * Use the standard C integer types
 */

#ifndef __ENVIRON_H
#define __ENVIRON_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

typedef int8_t    int8;
typedef uint8_t   uint8;

typedef int16_t   int16;
typedef uint16_t  uint16;

typedef int32_t   int32;
typedef uint32_t  uint32;

typedef int64_t   int64;
typedef uint64_t  uint64;

/* for parsing */
#define I32  "%"PRIi32
#define UI32 "%"PRIu32

#define I64  "%"PRIi64
#define UI64 "%"PRIu64

#endif /* ENVIRON_H */
