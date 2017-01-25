/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/genutil/ieee.h

#ifndef IEEE_H
#define IEEE_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int copy_bits (u_char *out, u_int out_offset, u_char *in, 
	       u_int in_offset, int num);
int cnvrt_long_double (u_char *data, double *integer, double *fraction);

#ifdef __cplusplus
	   }
#endif

#endif /* IEEE_H */
