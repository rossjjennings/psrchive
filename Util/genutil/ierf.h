/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/ierf.h,v $
   $Revision: 1.2 $
   $Date: 1999/11/02 09:18:11 $
   $Author: straten $ */

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

