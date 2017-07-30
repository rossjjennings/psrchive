/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/genutil/ierf.h

#ifndef __IERF_H
#define __IERF_H

#define MAX_ITERATIONS 40  /* maximum iterations toward answer */

#ifdef __cplusplus
extern "C" {
#endif

  /* Computes the inverse error function using the Newton-Raphson method */
  double ierf (double erfx);


#ifdef __cplusplus
	   }
#endif

#endif

