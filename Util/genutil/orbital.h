/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/orbital.h,v $
   $Revision: 1.2 $
   $Date: 2001/02/12 01:56:45 $
   $Author: straten $ */

#ifndef __ORBITAL_H
#define __ORBITAL_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Solves Kepler's equation using the Newton-Raphson method */
  double eccentric_anomoly (double mean_anomoly, double eccentricity);

  /* Solves mass function for the companion mass */
  double companion_mass (double mass_function, double sini, double m1);

#ifdef __cplusplus
	   }
#endif

#endif

