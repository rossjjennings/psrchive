/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/normal.h,v $
   $Revision: 1.2 $
   $Date: 2007/02/12 17:06:40 $
   $Author: straten $ */

#ifndef __genutil_normal_H
#define __genutil_normal_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Returns the standard normal probability function */
  double normal_probability (double x);

  /* Returns the integral of the probability density from 0 to x */
  double normal_distribution (double x);

  /* Returns the standard normal cummulative distribution function */
  double normal_cummulative (double x);

  /* Returns the expectation value of x^2, for x < threshold */
  double normal_moment2 (double threshold);

#ifdef __cplusplus
           }
#endif

#endif

