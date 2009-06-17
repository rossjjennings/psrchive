/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/environ.h,v $
   $Revision: 1.18 $
   $Date: 2009/06/17 08:12:20 $
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

/* more convenient when parsing */
#define I32  "%"PRIi32
#define UI32 "%"PRIu32

#define I64  "%"PRIi64
#define UI64 "%"PRIu64

#endif /* ENVIRON_H */
