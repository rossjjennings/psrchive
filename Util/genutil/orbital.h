/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/orbital.h,v $
   $Revision: 1.1 $
   $Date: 1999/12/10 04:40:17 $
   $Author: straten $ */

#ifndef __ORBITAL_H
#define __ORBITAL_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Solves Kepler's equation using the Newton-Raphson method */
  double eccentric_anomoly (double mean_anomoly, double eccentricity);

#ifdef __cplusplus
	   }
#endif

#endif

